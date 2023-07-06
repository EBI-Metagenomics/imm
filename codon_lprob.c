#include "codon_lprob.h"
#include "lprob.h"
#include "nuclt.h"
#include <assert.h>

struct imm_codon_lprob imm_codon_lprob(struct imm_nuclt const *nuclt)
{
  struct imm_codon_lprob lprob;
  lprob.nuclt = nuclt;

  unsigned i = 0;
  for (; i < IMM_NUCLT_SIZE * IMM_NUCLT_SIZE * IMM_NUCLT_SIZE; ++i)
    lprob.lprobs[i] = imm_lprob_zero();

  return lprob;
}

float imm_codon_lprob_get(struct imm_codon_lprob const *codonp,
                          struct imm_codon codon)
{
  assert(codonp->nuclt == codon.nuclt);
  return codonp->lprobs[IMM_CODON_STRIDE(codon.a, codon.b, codon.c)];
}

void imm_codon_lprob_set(struct imm_codon_lprob *codonp, struct imm_codon codon,
                         float lprob)
{
  assert(codonp->nuclt == codon.nuclt);
  codonp->lprobs[IMM_CODON_STRIDE(codon.a, codon.b, codon.c)] = lprob;
}

void imm_codon_lprob_normalize(struct imm_codon_lprob *codonp)
{
  imm_lprob_normalize(IMM_NUCLT_SIZE * IMM_NUCLT_SIZE * IMM_NUCLT_SIZE,
                      codonp->lprobs);
}
