#include "state.h"
#include "common/common.h"

struct imm_state *state_new(unsigned id, struct imm_abc const *abc,
                            struct imm_state_vtable vtable, unsigned min_seqlen,
                            unsigned max_seqlen)
{
    struct imm_state *state = xmalloc(sizeof(*state));
    state->id = id;
    state->idx = IMM_STATE_NULL_IDX;
    state->abc = abc;
    state->seqlen.min = min_seqlen;
    state->seqlen.max = max_seqlen;
    state->vtable = vtable;
    stack_init(&state->trans.outgoing);
    stack_init(&state->trans.incoming);
    hnode_init(&state->hnode);
    state->mark = 0;
    return state;
}

void state_del(struct imm_state const *state) { free((void *)state); }
