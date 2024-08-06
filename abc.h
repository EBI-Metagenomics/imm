#ifndef IMM_ABC_H
#define IMM_ABC_H

#include "str.h"
#include "sym.h"

#define IMM_ABC_MAX_SIZE 31
#define IMM_ABC_TYPEID_SIZE 10

enum imm_abc_typeid
{
  IMM_NULL_ABC = 0,
  IMM_ABC = 1,
  IMM_AMINO = 2,
  IMM_NUCLT = 3,
  IMM_DNA = 4,
  IMM_RNA = 5,
};

/**
 * Alphabet.
 *
 * It represents a finite set of symbols and a special any-symbol
 * symbol. It should be used as an immutable object.
 */
struct imm_abc
{
  int typeid;
  int size;
  char symbols[IMM_ABC_MAX_SIZE + 1];
  struct imm_sym sym;
  int any_symbol_id;
};

struct lio_writer;
struct lio_reader;

// clang-format off
IMM_API int         imm_abc_init(struct imm_abc *, struct imm_str symbols,
                                 char any_symbol_id);
IMM_API char        imm_abc_any_symbol(struct imm_abc const *);
IMM_API bool        imm_abc_has_symbol_id(struct imm_abc const *, int id);
IMM_API bool        imm_abc_has_symbol(struct imm_abc const *, char symbol);
IMM_API int         imm_abc_size(struct imm_abc const *);
IMM_API int         imm_abc_symbol_idx(struct imm_abc const *, char symbol);
IMM_API int         imm_abc_any_symbol_id(struct imm_abc const *);
IMM_API int         imm_abc_any_symbol_idx(struct imm_abc const *);
IMM_API int         imm_abc_symbol_type(struct imm_abc const *, char symbol);
IMM_API char const *imm_abc_symbols(struct imm_abc const *);
IMM_API int         imm_abc_union_size(struct imm_abc const *, struct imm_str seq);
IMM_API bool        imm_abc_typeid_valid(int typeid);
IMM_API char const *imm_abc_typeid_name(int typeid);
IMM_API int         imm_abc_pack(struct imm_abc const *, struct lio_writer *);
IMM_API int         imm_abc_unpack(struct imm_abc *, struct lio_reader *);
// clang-format on

int imm__abc_symbol_idx(struct imm_abc const *, int id);
int imm__abc_symbol_type(struct imm_abc const *, int id);
int imm__abc_init(struct imm_abc *, int len, char const *symbols,
                  char any_symbol, int);

#endif
