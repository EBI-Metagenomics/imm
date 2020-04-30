#ifndef STATE_H
#define STATE_H

#include "imm/state.h"
#include <stdio.h>

struct imm_abc;
struct imm_io;
struct imm_state;

static inline int state_write(struct imm_state const* state, struct imm_io const* io, FILE* stream)
{
    return state->vtable.write(state, io, stream);
}

#endif
