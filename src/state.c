#include "imm/state.h"
#include "error.h"

struct imm_state __imm_state_init(unsigned id, struct imm_abc const *abc,
                                  struct imm_state_vtable vtable,
                                  struct imm_span span)
{
    assert(id != IMM_STATE_NULL_ID);
    struct imm_state state;
    state.id = id;
    state.idx = IMM_STATE_NULL_IDX;
    state.abc = abc;
    state.span = span;
    state.vtable = vtable;
    cco_stack_init(&state.trans.outgoing);
    cco_stack_init(&state.trans.incoming);
    cco_hnode_init(&state.hnode);
    state.mark = 0;
    return state;
}

imm_float imm_state_lprob(struct imm_state const *state,
                          struct imm_seq const *seq)
{
    if (imm_unlikely(state->abc != imm_seq_abc(seq)))
    {
        error(IMM_DIFFERENT_ABC);
        return IMM_LPROB_NAN;
    }
    return state->vtable.lprob(state, seq);
}

void imm_state_detach(struct imm_state *state)
{
    cco_stack_init(&state->trans.outgoing);
    cco_stack_init(&state->trans.incoming);
    cco_hash_del(&state->hnode);
}
