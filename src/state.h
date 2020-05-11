#ifndef STATE_H
#define STATE_H

#include "imm/state.h"
#include <stdio.h>

struct imm_abc;
struct imm_model;
struct imm_state;

static inline int state_write(struct imm_state const* state, struct imm_model const* entry,
                              FILE* stream)
{
    return state->vtable.write(state, entry, stream);
}

#endif
