#include "amino_lprob.h"
#include <assert.h>

struct imm_amino_lprob imm_amino_lprob(struct imm_amino const *amino,
                                       float const *lprobs)
{
  struct imm_amino_lprob lprob;
  lprob.amino = amino;
  for (unsigned i = 0; i < IMM_AMINO_SIZE; ++i)
    lprob.lprobs[i] = lprobs[i];
  return lprob;
}

float imm_amino_lprob_get(struct imm_amino_lprob const *lprob, char symbol)
{
  struct imm_abc const *abc = &lprob->amino->super;
  assert(imm_abc_has_symbol(abc, symbol));
  return imm__amino_lprob_get(lprob, imm_abc_symbol_idx(abc, symbol));
}

float imm__amino_lprob_get(struct imm_amino_lprob const *lprob, unsigned idx)
{
  return lprob->lprobs[idx];
}
