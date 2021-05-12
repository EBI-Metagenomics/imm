#ifndef IMM_STEP_H
#define IMM_STEP_H

#include "imm/export.h"
#include "imm/state_types.h"
#include <assert.h>
#include <stdalign.h>
#include <stdint.h>

struct imm_step
{
    imm_state_id_t state_id;
    imm_state_seqlen_t seqlen;
};

static_assert(sizeof(struct imm_step) == 4, "struct pair must be packed");
static_assert(alignof(struct imm_step) == 2, "struct pair must align to 2");

static inline struct imm_step IMM_STEP(unsigned state_id, unsigned seqlen)
{
    return (struct imm_step){(imm_state_id_t)state_id,
                             (imm_state_seqlen_t)seqlen};
}

#endif
