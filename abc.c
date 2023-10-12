#include "abc.h"
#include "array_size.h"
#include "expect.h"
#include "likely.h"
#include "lip/1darray/1darray.h"
#include "lip/file/file.h"
#include "lip/lip.h"
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

bool imm_abc_has_symbol_id(struct imm_abc const *abc, unsigned id)
{
  if (imm_unlikely(!imm_sym_valid_id(id))) return false;

  return imm_sym_idx(&abc->sym, id) < abc->size;
}

bool imm_abc_has_symbol(struct imm_abc const *abc, char symbol)
{
  return imm_abc_has_symbol_id(abc, imm_sym_id(symbol));
}

unsigned imm_abc_size(struct imm_abc const *abc) { return abc->size; }

unsigned imm_abc_symbol_idx(struct imm_abc const *abc, char symbol)
{
  return imm__abc_symbol_idx(abc, imm_sym_id(symbol));
}

unsigned imm_abc_any_symbol_id(struct imm_abc const *abc)
{
  return abc->any_symbol_id;
}

unsigned imm_abc_any_symbol_idx(struct imm_abc const *abc) { return abc->size; }

enum imm_sym_type imm_abc_symbol_type(struct imm_abc const *abc, char symbol)
{
  return imm__abc_symbol_type(abc, imm_sym_id(symbol));
}

char const *imm_abc_symbols(struct imm_abc const *abc) { return abc->symbols; }

unsigned imm_abc_union_size(struct imm_abc const *abc, struct imm_str seq)
{
  unsigned size = 0;
  for (unsigned i = 0; i < seq.size; ++i)
  {
    char const any = imm_abc_any_symbol(abc);
    size += !imm_abc_has_symbol(abc, seq.data[i]) && seq.data[i] != any;
  }
  return size;
}

bool imm_abc_typeid_valid(unsigned typeid)
{
  return typeid > IMM_NULL_ABC && typeid <= IMM_RNA;
}

char const *imm_abc_typeid_name(enum imm_abc_typeid typeid)
{
  static char const abc_typeid_name[][9] = {"null_abc", "abc", "amino",
                                            "nuclt",    "dna", "rna"};
  return abc_typeid_name[typeid];
}

unsigned imm__abc_symbol_idx(struct imm_abc const *abc, unsigned id)
{
  assert(imm_abc_has_symbol_id(abc, id) || id == abc->any_symbol_id);
  return imm_sym_idx(&abc->sym, id);
}

enum imm_sym_type imm__abc_symbol_type(struct imm_abc const *abc, unsigned id)
{
  if (id == abc->any_symbol_id) return IMM_SYM_ANY;

  if (imm_abc_has_symbol_id(abc, id)) return IMM_SYM_NORMAL;

  return IMM_SYM_NULL;
}

int imm__abc_init(struct imm_abc *x, unsigned len, char const *symbols,
                  char any_symbol, enum imm_abc_typeid typeid)
{
  x->typeid = typeid;
  if (!imm_sym_valid_char(any_symbol)) return IMM_EINVAL;

  if (len == 0) return IMM_EINVAL;

  if (len > IMM_ABC_MAX_SIZE || len > IMM_SYM_SIZE) return IMM_EMANYSYMBOLS;

  x->size = len;
  memcpy(x->symbols, symbols, sizeof *x->symbols * len);
  x->symbols[x->size] = '\0';
  imm_sym_init(&x->sym);
  x->any_symbol_id = imm_sym_id(any_symbol);

  for (unsigned i = 0; i < x->size; ++i)
  {
    if (symbols[i] == any_symbol) return IMM_EINVAL;

    if (!imm_sym_valid_char(symbols[i])) return IMM_EINVAL;

    unsigned id = imm_sym_id(symbols[i]);
    if (imm_sym_idx(&x->sym, id) != IMM_SYM_NULL_IDX) return IMM_EINVAL;

    imm_sym_set_idx(&x->sym, id, i);
  }
  imm_sym_set_idx(&x->sym, x->any_symbol_id, x->size);
  return 0;
}

static int abc_pack(struct imm_abc const *x, struct lip_file *file)
{
  lip_write_map_size(file, 4);

  lip_write_cstr(file, "symbols");
  lip_write_cstr(file, x->symbols);

  lip_write_cstr(file, "idx");
  lip_write_1darray_u8(file, imm_array_size(x->sym.idx), x->sym.idx);

  lip_write_cstr(file, "any_symbol_id");
  lip_write_int(file, x->any_symbol_id);

  lip_write_cstr(file, "typeid");
  lip_write_int(file, x->typeid);

  return file->error ? IMM_EIO : 0;
}

static int abc_unpack(struct imm_abc *x, struct lip_file *file)
{
  if (!imm_expect_map_size(file, 4)) return IMM_EIO;

  if (!imm_expect_map_key(file, "symbols")) return IMM_EIO;
  lip_read_cstr(file, IMM_ABC_MAX_SIZE, x->symbols);

  x->size = (unsigned)strlen(x->symbols);

  if (!imm_expect_map_key(file, "idx")) return IMM_EIO;
  if (!imm_expect_1darr_u8_type(file, imm_array_size(x->sym.idx), x->sym.idx))
    return IMM_EIO;

  if (!imm_expect_map_key(file, "any_symbol_id")) return IMM_EIO;
  lip_read_int(file, &x->any_symbol_id);

  if (!imm_expect_map_key(file, "typeid")) return IMM_EIO;
  lip_read_int(file, &x->typeid);

  if (!imm_abc_typeid_valid(x->typeid)) return IMM_EIO;

  return file->error ? IMM_EIO : 0;
}

int imm_abc_pack(struct imm_abc const *abc, struct lip_file *file)
{
  int rc = abc_pack(abc, file);
  return rc ? IMM_EIO : rc;
}

int imm_abc_unpack(struct imm_abc *abc, struct lip_file *file)
{
  int rc = abc_unpack(abc, file);
  return rc ? IMM_EIO : rc;
}
