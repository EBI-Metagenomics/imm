#ifndef STATE_H
#define STATE_H

#include "imm/state.h"
#include <stdio.h>

struct imm_state;
struct imm_abc;

static inline int state_write(struct imm_state const* state, FILE* stream)
{
    return state->vtable.write(state, stream);
}
int               state_write_parent(struct imm_state const* state, FILE* stream);
struct imm_state* state_read_parent(FILE* stream, struct imm_abc const* abc);

#endif
