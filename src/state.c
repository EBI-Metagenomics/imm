#include "state.h"
#include "common/common.h"

struct imm_state *imm_state_new(imm_state_id_t id, struct imm_abc const *abc,
                                struct imm_state_vtable vtable)
{
    struct imm_state *state = xmalloc(sizeof(*state));
    state->id = id;
    state->idx = IMM_STATE_NULL_IDX;
    state->abc = abc;
    state->vtable = vtable;
    stack_init(&state->trans.outgoing);
    stack_init(&state->trans.incoming);
    hnode_init(&state->hnode);
    return state;
}

void state_del(struct imm_state const *state) { free((void *)state); }
