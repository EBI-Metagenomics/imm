#ifndef DP_TRANS_INFO_H
#define DP_TRANS_INFO_H

#include "imm/float.h"
#include "imm/lprob.h"
#include "imm/state_types.h"
#include <stdint.h>

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

#endif
