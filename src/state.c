#include "state.h"
#include "support.h"

struct imm_state *state_new(unsigned id, struct imm_abc const *abc,
                            struct imm_state_vtable vtable,
                            struct imm_span span)
{
    IMM_BUG(id >= IMM_STATE_NULL_ID);
    struct imm_state *state = xmalloc(sizeof(*state));
    state->id = id;
    state->idx = IMM_STATE_NULL_IDX;
    state->abc = abc;
    state->span = span;
    state->vtable = vtable;
    cco_stack_init(&state->trans.outgoing);
    cco_stack_init(&state->trans.incoming);
    cco_hnode_init(&state->hnode);
    state->mark = 0;
    return state;
}

void state_del(struct imm_state const *state) { free((void *)state); }
