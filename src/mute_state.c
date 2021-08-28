#include "imm/mute_state.h"
#include "state.h"

static imm_float lprob(struct imm_state const *state,
                       struct imm_seq const *seq);

void imm_mute_state_init(struct imm_mute_state *state, unsigned id,
                         struct imm_abc const *abc)
{
    struct imm_state_vtable vtable = {lprob, IMM_MUTE_STATE, state};
    state->super = __imm_state_init(id, abc, vtable, IMM_SPAN(0, 0));
}

static imm_float lprob(struct imm_state const *state, struct imm_seq const *seq)
{
    if (imm_seq_size(seq) == 0)
        return imm_log(1.0);
    return imm_lprob_zero();
}
