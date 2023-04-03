#ifndef IMM_AMINO_LPROB_H
#define IMM_AMINO_LPROB_H

#include "amino.h"
#include "export.h"
#include "lprob.h"

struct imm_amino_lprob
{
  struct imm_amino const *amino;
  float lprobs[IMM_AMINO_SIZE];
};

IMM_API struct imm_amino_lprob imm_amino_lprob(struct imm_amino const *,
                                               float const *lprobs);
IMM_API float imm_amino_lprob_get(struct imm_amino_lprob const *, char symbol);
float imm__amino_lprob_get(struct imm_amino_lprob const *, unsigned idx);

#endif
