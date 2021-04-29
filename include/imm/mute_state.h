#ifndef IMM_MUTE_STATE_H
#define IMM_MUTE_STATE_H

#include "imm/export.h"
#include "imm/state_types.h"

struct imm_abc;

struct imm_mute_state
{
    struct imm_state *super;
};

IMM_API struct imm_mute_state *imm_mute_state_new(imm_state_id_t id,
                                                  struct imm_abc const *abc);

IMM_API void imm_mute_state_del(struct imm_mute_state const *mute);

static inline struct imm_state const *
imm_mute_state_super(struct imm_mute_state const *mute)
{
    return mute->super;
}

#endif
