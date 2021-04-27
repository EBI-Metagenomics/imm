#ifndef IMM_STEP_H
#define IMM_STEP_H

#include "imm/export.h"
#include "imm/state.h"
#include <stdint.h>

struct imm_step
{
    stateid_t state_id;
    uint8_t seq_len;
};

#define IMM_STEP(state_id, seq_len)                                            \
    (struct imm_step) { (state_id), (seq_len) }

#endif
