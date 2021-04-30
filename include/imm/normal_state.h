#ifndef IMM_NORMAL_STATE_H
#define IMM_NORMAL_STATE_H

#include "imm/export.h"
#include "imm/float.h"
#include "imm/state_types.h"
#include <stdint.h>
#include <stdio.h>

struct imm_abc;

struct imm_normal_state
{
    struct imm_state *super;
    imm_float const *lprobs;
};

IMM_API struct imm_normal_state *imm_normal_state_new(imm_state_id_t id,
                                                      struct imm_abc const *abc,
                                                      imm_float const lprobs[]);

IMM_API void imm_normal_state_del(struct imm_normal_state const *normal);

static inline struct imm_state *
imm_normal_state_super(struct imm_normal_state *normal)
{
    return normal->super;
}

#endif
