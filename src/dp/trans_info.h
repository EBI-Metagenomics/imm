#ifndef DP_TRANS_INFO_H
#define DP_TRANS_INFO_H

#include "imm/float.h"
#include "imm/lprob.h"
#include "imm/state_types.h"
#include <stdint.h>
#include <stdio.h>

struct trans_info
{
    imm_float score;
    unsigned prev_state;
    unsigned prev_seqlen;
    uint_fast16_t trans;
    uint_fast8_t seqlen;
};

static inline struct trans_info trans_info(void)
{
    return (struct trans_info){imm_lprob_zero(), IMM_STATE_NULL_IDX,
                               IMM_STATE_NULL_SEQLEN, UINT16_MAX, UINT8_MAX};
}

static inline void update_trans_info(struct trans_info *x, imm_float score,
                                     unsigned prev_state, unsigned prev_seqlen,
                                     uint_fast16_t trans, uint_fast8_t seqlen)
{
    // printf("    score[%f] > x->score[%f]: ", score, x->score);
    if (score > x->score)
    {
        // printf("yes\n");
        x->score = score;
        x->prev_state = prev_state;
        x->prev_seqlen = prev_seqlen;
        x->trans = trans;
        x->seqlen = seqlen;
    }
    // else
    //     printf("no\n");
}

#endif
