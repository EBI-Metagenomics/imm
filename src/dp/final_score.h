#ifndef DP_FINAL_SCORE_H
#define DP_FINAL_SCORE_H

#include "imm/float.h"
#include <stdint.h>

struct final_score2
{
    imm_float score;
    unsigned prev_state;
    unsigned prev_seqlen;
    uint16_t trans;
    uint8_t seqlen;
};

#endif
