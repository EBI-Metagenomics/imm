#ifndef IMM_MUTE_STATE_H
#define IMM_MUTE_STATE_H

#include "imm/export.h"
#include "imm/state.h"

struct imm_abc;

struct imm_mute_state
{
    struct imm_state super;
};

IMM_API void imm_mute_state_init(struct imm_mute_state *state, unsigned id,
                                 struct imm_abc const *abc);

static inline struct imm_state *
imm_mute_state_super(struct imm_mute_state *mute)
{
    return &mute->super;
}

static inline struct imm_state const *
imm_mute_state_super_c(struct imm_mute_state const *mute)
{
    return &mute->super;
}

#endif
