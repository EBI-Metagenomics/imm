#ifndef IMM_BTRANS_H
#define IMM_BTRANS_H

#include "imm/float.h"
#include "imm/lprob.h"
#include "imm/state_types.h"

struct imm_btrans
{
    imm_float score;
    unsigned prev_state;
    unsigned trans;
    unsigned len;
};

static inline struct imm_btrans imm_btrans_init(void)
{
    return (struct imm_btrans){imm_lprob_zero(), IMM_STATE_NULL_IDX,
                               IMM_STATE_NULL_SEQLEN, UINT16_MAX};
}

static inline void imm_btrans_set(struct imm_btrans *x, imm_float score,
                                  unsigned prev_state, unsigned trans,
                                  unsigned len)
{
    x->score = score;
    x->prev_state = prev_state;
    x->trans = trans;
    x->len = len;
}

#endif
