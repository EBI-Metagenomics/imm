#ifndef START_H
#define START_H

#include "imm/lprob.h"
#include "imm/state.h"

struct start
{
    imm_float lprob;
    unsigned state_id;
};

static inline void start_init(struct start *start)
{
    start->lprob = imm_lprob_nan();
    start->state_id = IMM_STATE_NULL_ID;
}

#endif
