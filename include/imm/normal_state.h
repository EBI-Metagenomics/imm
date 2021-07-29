#ifndef IMM_NORMAL_STATE_H
#define IMM_NORMAL_STATE_H

#include "imm/export.h"
#include "imm/state.h"

struct imm_abc;

struct imm_normal_state
{
    struct imm_state super;
    imm_float const *lprobs;
};

IMM_API void imm_normal_state_init(struct imm_normal_state *state, unsigned id,
                                   struct imm_abc const *abc,
                                   imm_float const lprobs[static 1]);

static inline struct imm_state *
imm_normal_state_super(struct imm_normal_state *normal)
{
    return &normal->super;
}

#endif
