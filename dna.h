#ifndef IMM_DNA_H
#define IMM_DNA_H

#include "compiler.h"
#include "nuclt.h"

#define IMM_DNA_ANY_SYMBOL 'X'
#define IMM_DNA_SYMBOLS "ACGT"
#define IMM_DNA_SIZE 4

struct imm_dna
{
  struct imm_nuclt super;
};

IMM_API extern struct imm_dna const imm_dna_iupac;

IMM_API int imm_dna_init(struct imm_dna *, char const *symbols,
                         char any_symbol);

#endif
