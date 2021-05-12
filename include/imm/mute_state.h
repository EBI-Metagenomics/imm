#ifndef IMM_MUTE_STATE_H
#define IMM_MUTE_STATE_H

#include "imm/export.h"
#include "imm/state.h"

struct imm_abc;

struct imm_mute_state
{
    struct imm_state *super;
};

IMM_API struct imm_mute_state *imm_mute_state_new(unsigned id,
                                                  struct imm_abc const *abc);

static inline void imm_mute_state_del(struct imm_mute_state const *mute)
{
    mute->super->vtable.del(mute->super);
}

static inline struct imm_state *
imm_mute_state_super(struct imm_mute_state *mute)
{
    return mute->super;
}

#endif
