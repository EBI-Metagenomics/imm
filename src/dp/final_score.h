#ifndef DP_FINAL_SCORE_H
#define DP_FINAL_SCORE_H

#include "imm/float.h"
#include <stdint.h>

struct final_score
{
    imm_float score;
    unsigned state;
    unsigned emislen;
    uint16_t trans;
    uint8_t seqlen;
};

#endif
