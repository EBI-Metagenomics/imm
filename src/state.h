#ifndef STATE_H
#define STATE_H

#include "imm/state.h"

struct imm_state *state_new(unsigned id, struct imm_abc const *abc,
                            struct imm_state_vtable vtable,
                            struct imm_span span);

static inline void state_detach(struct imm_state *state)
{
    stack_init(&state->trans.outgoing);
    stack_init(&state->trans.incoming);
    hash_del(&state->hnode);
}

void state_del(struct imm_state const *state);

#endif
