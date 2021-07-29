#ifndef STATE_H
#define STATE_H

#include "imm/state.h"
#include "support.h"

static inline void state_detach(struct imm_state *state)
{
    cco_stack_init(&state->trans.outgoing);
    cco_stack_init(&state->trans.incoming);
    cco_hash_del(&state->hnode);
}

#endif
