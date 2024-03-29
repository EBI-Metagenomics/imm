#ifndef IMM_NUCLT_H
#define IMM_NUCLT_H

#include "abc.h"
#include "compiler.h"

#define IMM_NUCLT_ANY_SYMBOL 'X'
#define IMM_NUCLT_SYMBOLS "ACGT"
#define IMM_NUCLT_SIZE 4

struct imm_nuclt
{
  struct imm_abc super;
};

IMM_API int imm_nuclt_init(struct imm_nuclt *, char const symbols[],
                           char any_symbol);
IMM_API int imm_nuclt_size(struct imm_nuclt const *);

#endif
