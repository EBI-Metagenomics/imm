#include "abc_lprob.h"
#include "abc.h"
#include <assert.h>

struct imm_abc_lprob imm_abc_lprob(struct imm_abc const *abc,
                                   float const *lprobs)
{
  return (struct imm_abc_lprob){abc, lprobs};
}

float imm_abc_lprob_get(struct imm_abc_lprob const *lprob, char symbol)
{
  assert(imm_abc_has_symbol(lprob->abc, symbol));
  return lprob->lprobs[imm_abc_symbol_idx(lprob->abc, symbol)];
}
