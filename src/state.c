#include "imm/state.h"
#include "error.h"

imm_float imm_state_lprob(struct imm_state const *state,
                          struct imm_seq const *seq)
{
    if (imm_unlikely(state->abc != imm_seq_abc(seq)))
    {
        error(IMM_ILLEGALARG, "alphabets must be the same");
        return IMM_LPROB_NAN;
    }
    return state->vtable.lprob(state, seq);
}
