#ifndef IMM_NORMAL_STATE_H
#define IMM_NORMAL_STATE_H

#include "imm/export.h"
#include "imm/state.h"
#include "imm/state_types.h"

struct imm_abc;

struct imm_normal_state
{
    struct imm_state *super;
    imm_float const *lprobs;
};

IMM_API struct imm_normal_state *
imm_normal_state_new(unsigned id, struct imm_abc const *abc,
                     imm_float const lprobs[static 1]);

static inline void imm_normal_state_del(struct imm_normal_state const *normal)
{
    normal->super->vtable.del(normal->super);
}

static inline struct imm_state *
imm_normal_state_super(struct imm_normal_state *normal)
{
    return normal->super;
}

#endif
