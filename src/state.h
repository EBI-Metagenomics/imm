#ifndef STATE_H
#define STATE_H

#include "imm/state.h"

static inline void state_detach(struct imm_state *state)
{
    IMM_INIT_LIST_HEAD(&state->trans.outgoing);
    IMM_INIT_LIST_HEAD(&state->trans.incoming);
    cco_hash_del(&state->hnode);
}

#endif
