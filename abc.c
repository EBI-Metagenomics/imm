#include "abc.h"
#include "array_size.h"
#include "likely.h"
#include "lite_pack_io.h"
#include "lite_pack.h"
#include "rc.h"
#include "sym.h"
#include <assert.h>
#include <string.h>

int imm_abc_init(struct imm_abc *abc, struct imm_str symbols, char any_symbol)
{
  return imm__abc_init(abc, symbols.size, symbols.data, any_symbol, IMM_ABC);
}

char imm_abc_any_symbol(struct imm_abc const *abc)
{
  return imm_sym_char(abc->any_symbol_id);
}

bool imm_abc_has_symbol_id(struct imm_abc const *abc, int id)
{
  if (imm_unlikely(!imm_sym_valid_id(id))) return false;

  return imm_sym_idx(&abc->sym, id) < abc->size;
}

bool imm_abc_has_symbol(struct imm_abc const *abc, char symbol)
{
  return imm_abc_has_symbol_id(abc, imm_sym_id(symbol));
}

int imm_abc_size(struct imm_abc const *abc) { return abc->size; }

int imm_abc_symbol_idx(struct imm_abc const *abc, char symbol)
{
  return imm__abc_symbol_idx(abc, imm_sym_id(symbol));
}

int imm_abc_any_symbol_id(struct imm_abc const *abc)
{
  return abc->any_symbol_id;
}

int imm_abc_any_symbol_idx(struct imm_abc const *abc) { return abc->size; }

int imm_abc_symbol_type(struct imm_abc const *abc, char symbol)
{
  return imm__abc_symbol_type(abc, imm_sym_id(symbol));
}

char const *imm_abc_symbols(struct imm_abc const *abc) { return abc->symbols; }

int imm_abc_union_size(struct imm_abc const *abc, struct imm_str seq)
{
  int size = 0;
  for (int i = 0; i < seq.size; ++i)
  {
    char const any = imm_abc_any_symbol(abc);
    size += !imm_abc_has_symbol(abc, seq.data[i]) && seq.data[i] != any;
  }
  return size;
}

bool imm_abc_typeid_valid(int typeid)
{
  return typeid > IMM_NULL_ABC && typeid <= IMM_RNA;
}

char const *imm_abc_typeid_name(int typeid)
{
  static char const abc_typeid_name[][9] = {"null_abc", "abc", "amino",
                                            "nuclt",    "dna", "rna"};
  return abc_typeid_name[typeid];
}

int imm__abc_symbol_idx(struct imm_abc const *abc, int id)
{
  assert(imm_abc_has_symbol_id(abc, id) || id == abc->any_symbol_id);
  return imm_sym_idx(&abc->sym, id);
}

int imm__abc_symbol_type(struct imm_abc const *abc, int id)
{
  if (id == abc->any_symbol_id) return IMM_SYM_ANY;

  if (imm_abc_has_symbol_id(abc, id)) return IMM_SYM_NORMAL;

  return IMM_SYM_NULL;
}

int imm__abc_init(struct imm_abc *x, int size, char const *symbols,
                  char any_symbol, int typeid)
{
  assert(size >= 0);
  x->typeid = typeid;
  if (!imm_sym_valid_char(any_symbol)) return IMM_EINVAL;

  if (size == 0) return IMM_EINVAL;

  if (size > IMM_ABC_MAX_SIZE || size > IMM_SYM_SIZE) return IMM_EMANYSYMBOLS;

  x->size = size;
  memcpy(x->symbols, symbols, sizeof(*x->symbols) * (unsigned)size);
  x->symbols[x->size] = '\0';
  imm_sym_init(&x->sym);
  x->any_symbol_id = imm_sym_id(any_symbol);

  for (int i = 0; i < x->size; ++i)
  {
    if (symbols[i] == any_symbol) return IMM_EINVAL;

    if (!imm_sym_valid_char(symbols[i])) return IMM_EINVAL;

    int id = imm_sym_id(symbols[i]);
    if (imm_sym_idx(&x->sym, id) != IMM_SYM_NULL_IDX) return IMM_EINVAL;

    imm_sym_set_idx(&x->sym, id, i);
  }
  imm_sym_set_idx(&x->sym, x->any_symbol_id, x->size);
  return 0;
}

static int write_cstring(struct lio_writer *x, char const *string)
{
  uint32_t length = (uint32_t)strlen(string);
  if (lio_write(x, lip_pack_string(lio_alloc(x), length))) return 1;
  if (lio_writeb(x, length, string)) return 1;
  return 0;
}

static int expect_key(struct lio_reader *x, char const *key)
{
  uint32_t size = 0;
  unsigned char buf[16] = {0};

  if (lio_free(x, lip_unpack_string(lio_read(x), &size))) return 1;
  if ((size_t)size > sizeof(buf)) return 1;

  if (lio_readb(x, size, buf)) return 1;
  if (size != (uint32_t)strlen(key)) return 1;
  return memcmp(key, buf, size) != 0;
}

int imm_abc_pack(struct imm_abc const *x, struct lio_writer *f)
{
  if (lio_write(f, lip_pack_map(lio_alloc(f), 4))) return IMM_EIO;

  if (write_cstring(f, "symbols")) return IMM_EIO;
  if (write_cstring(f, x->symbols)) return IMM_EIO;

  if (write_cstring(f, "idx")) return IMM_EIO;
  if (lio_write(f, lip_pack_array(lio_alloc(f), imm_array_size(x->sym.idx)))) return IMM_EIO;
  for (size_t i = 0; i < imm_array_size(x->sym.idx); ++i)
    if (lio_write(f, lip_pack_int(lio_alloc(f), x->sym.idx[i]))) return IMM_EIO;

  if (write_cstring(f, "any_symbol_id")) return IMM_EIO;
  if (lio_write(f, lip_pack_int(lio_alloc(f), x->any_symbol_id))) return IMM_EIO;

  if (write_cstring(f, "typeid")) return IMM_EIO;
  if (lio_write(f, lip_pack_int(lio_alloc(f), x->typeid))) return IMM_EIO;

  if (lio_flush(f)) return IMM_EIO;

  return 0;
}

int imm_abc_unpack(struct imm_abc *x, struct lio_reader *f)
{
  uint32_t u32 = 0;
  if (lio_free(f, lip_unpack_map(lio_read(f), &u32))) return IMM_EIO;
  if (u32 != 4) return IMM_EIO;

  if (expect_key(f, "symbols")) return IMM_EIO;
  if (lio_free(f, lip_unpack_string(lio_read(f), &u32))) return IMM_EIO;
  if (u32 > IMM_ABC_MAX_SIZE) return IMM_EIO;
  if (lio_readb(f, u32, (unsigned char *)x->symbols)) return IMM_EIO;
  x->symbols[u32] = '\0';

  x->size = (int)strlen(x->symbols);

  if (expect_key(f, "idx")) return IMM_EIO;
  if (lio_free(f, lip_unpack_array(lio_read(f), &u32))) return IMM_EIO;
  if ((size_t)u32 != imm_array_size(x->sym.idx)) return IMM_EIO;
  for (uint32_t i = 0; i < u32; i++)
    if (lio_free(f, lip_unpack_int(lio_read(f), x->sym.idx + i))) return IMM_EIO;

  if (expect_key(f, "any_symbol_id")) return IMM_EIO;
  if (lio_free(f, lip_unpack_int(lio_read(f), &x->any_symbol_id))) return IMM_EIO;

  if (expect_key(f, "typeid")) return IMM_EIO;
  if (lio_free(f, lip_unpack_int(lio_read(f), &x->typeid))) return IMM_EIO;

  if (!imm_abc_typeid_valid(x->typeid)) return IMM_EIO;

  return 0;
}
