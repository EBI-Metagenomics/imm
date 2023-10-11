#ifndef IMM_SEQ_H
#define IMM_SEQ_H

#include "api.h"
#include "str.h"

struct imm_abc;

struct imm_seq
{
  unsigned size;
  char const *str;
  struct imm_abc const *abc;
};

IMM_API struct imm_seq imm_seq_unsafe(unsigned size, char const *,
                                      struct imm_abc const *);
IMM_API struct imm_seq imm_seq(struct imm_str, struct imm_abc const *);
IMM_API int imm_seq_init(struct imm_seq *, struct imm_str, struct imm_abc const *);
IMM_API struct imm_abc const *imm_seq_abc(struct imm_seq const *);
IMM_API unsigned imm_seq_size(struct imm_seq const *);
IMM_API char const *imm_seq_str(struct imm_seq const *);
IMM_API unsigned imm_seq_symbol_idx(struct imm_seq const *, unsigned i);

#endif
