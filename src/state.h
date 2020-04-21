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
int               state_write_base(struct imm_state const* state, FILE* stream);
struct imm_state* state_read_base(FILE* stream, struct imm_abc const* abc);
void              state_destroy(struct imm_state const* state);

#endif
