#ifndef IMM_AMINO_H
#define IMM_AMINO_H

#include "abc.h"
#include "compiler.h"

#define IMM_AMINO_ANY_SYMBOL 'X'
#define IMM_AMINO_SYMBOLS "ACDEFGHIKLMNPQRSTVWY"
#define IMM_AMINO_SIZE 20

struct imm_amino
{
  struct imm_abc super;
};

IMM_API extern struct imm_amino const imm_amino_iupac;

IMM_API int imm_amino_init(struct imm_amino *amino, char const *symbols,
                           char any_symbol);

#endif
