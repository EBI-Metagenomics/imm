#include "btrans.h"

struct imm_btrans imm_btrans_init(void)
{
  return (struct imm_btrans){imm_lprob_zero(), IMM_STATE_NULL_IDX,
                             IMM_STATE_NULL_SEQLEN, UINT16_MAX};
}

void imm_btrans_set(struct imm_btrans *x, float score, unsigned prev_state,
                    unsigned trans, unsigned len)
{
  x->score = score;
  x->prev_state = prev_state;
  x->trans = trans;
  x->len = len;
}
