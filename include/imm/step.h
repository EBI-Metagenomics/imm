#ifndef IMM_STEP_H
#define IMM_STEP_H

#include "imm/export.h"
#include "imm/state.h"
#include <assert.h>
#include <stdalign.h>
#include <stdint.h>

struct imm_step
{
    imm_state_id_t state_id;
    uint8_t seqlen;
};

static_assert(sizeof(struct imm_step) == 4, "struct pair must be packed");
static_assert(alignof(struct imm_step) == 2, "struct pair must align to 1");

#define IMM_STEP(state_id, seqlen)                                             \
    (struct imm_step)                                                          \
    {                                                                          \
        (imm_state_id_t)(state_id), (imm_state_seqlen_t)(seqlen)               \
    }

#endif
