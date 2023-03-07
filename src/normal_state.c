#include "imm/normal_state.h"
#include "imm/abc.h"
#include "state.h"

static imm_float lprob(struct imm_state const *state,
                       struct imm_seq const *seq);

void imm_normal_state_init(struct imm_normal_state *state, unsigned id,
                           struct imm_abc const *abc,
                           imm_float const lprobs[static 1])
{
    state->lprobs = lprobs;
    struct imm_state_vtable vtable = {lprob, IMM_NORMAL_STATE, state};
    __imm_state_init(&state->super, id, abc, vtable, imm_span(1, 1));
}

static imm_float lprob(struct imm_state const *state, struct imm_seq const *seq)
{
    struct imm_normal_state const *normal = state->vtable.derived;
    if (imm_seq_size(seq) == 1)
    {
        struct imm_abc const *abc = imm_state_abc(state);
        unsigned idx = imm_abc_symbol_idx(abc, seq->str[0]);
        if (idx != IMM_SYM_NULL_IDX) return normal->lprobs[idx];
    }
    return imm_lprob_zero();
}
