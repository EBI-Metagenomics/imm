#ifndef IMM_NORMAL_STATE_H
#define IMM_NORMAL_STATE_H

#include "imm/export.h"
#include "imm/float.h"
#include "imm/state.h"
#include "imm/state_types.h"
#include <stdint.h>
#include <stdio.h>

/** @file normal_state.h
 * Normal state module.
 *
 * An object of type @ref imm_normal_state is meant to be immutable.
 */

struct imm_abc;
/* struct imm_profile; */
struct imm_state;

struct imm_normal_state
{
    struct imm_state *super;
    imm_float *lprobs;
};

IMM_API struct imm_normal_state *imm_normal_state_new(uint16_t id,
                                                      struct imm_abc const *abc,
                                                      imm_float const *lprobs);

static inline struct imm_normal_state *imm_normal_state(struct imm_state *state)
{
    if (imm_state_type_id(state) != IMM_NORMAL_STATE_TYPE_ID)
    {
        imm_log_error("could not cast to normal_state");
        return NULL;
    }
    return __imm_state_derived(state);
}

static inline void imm_normal_state_del(struct imm_normal_state const *state)
{
    state->super->vtable.del(state->super);
}

IMM_API struct imm_state *imm_normal_state_read(FILE *stream,
                                                struct imm_abc const *abc);

static inline struct imm_state *
imm_normal_state_super(struct imm_normal_state *state)
{
    return state->super;
}

/* IMM_API int imm_normal_state_write(struct imm_state const *state, */
/*                                    struct imm_profile const *prof, */
/*                                    FILE *stream); */

#endif
