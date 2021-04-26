#ifndef START_H
#define START_H

#include "imm/lprob.h"
#include "imm/state.h"

struct start
{
    imm_float lprob;
    uint16_t state_id;
};

static inline void start_init(struct start *start)
{
    start->lprob = imm_lprob_invalid();
    start->state_id = IMM_INVALID_STATE_ID;
}

#endif
