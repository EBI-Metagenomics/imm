#ifndef IMM_MUTE_STATE_H
#define IMM_MUTE_STATE_H

#include "imm/export.h"
#include "imm/log.h"
#include "imm/state.h"
#include "imm/state_types.h"
#include <stdint.h>
#include <stdio.h>

/** @file mute_state.h
 * Mute state module.
 *
 * An object of type @ref imm_mute_state is meant to be immutable.
 */

struct imm_abc;
/* struct imm_profile; */

struct imm_mute_state
{
    struct imm_state *super;
};

IMM_API struct imm_mute_state *imm_mute_state_new(uint16_t id,
                                                  struct imm_abc const *abc);

static inline struct imm_mute_state *imm_mute_state(struct imm_state *state)
{
    if (imm_state_type_id(state) != IMM_MUTE_STATE_TYPE_ID)
    {
        imm_log_error("could not cast to mute_state");
        return NULL;
    }
    return __imm_state_derived(state);
}

static inline void imm_mute_state_del(struct imm_mute_state const *state)
{
    state->super->vtable.del(state->super);
}

IMM_API struct imm_state *imm_mute_state_read(FILE *stream,
                                              struct imm_abc const *abc);

static inline struct imm_state *
imm_mute_state_super(struct imm_mute_state *state)
{
    return state->super;
}

static inline void imm_mute_state_write(struct imm_state const *state,
                                        struct imm_profile const *prof,
                                        FILE *stream)
{
    __imm_state_write(state, stream);
}

#endif
