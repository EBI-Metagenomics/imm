#include "imm/mute_state.h"
#include "common/common.h"
#include "state.h"

static void del(struct imm_state const *state);

static imm_float lprob(struct imm_state const *state,
                       struct imm_seq const *seq);

struct imm_mute_state *imm_mute_state_new(unsigned id,
                                          struct imm_abc const *abc)
{
    struct imm_mute_state *mute = xmalloc(sizeof(*mute));
    struct imm_state_vtable vtable = {del, lprob, IMM_MUTE_STATE, mute};
    mute->super = state_new(id, abc, vtable, 0, 0);
    return mute;
}

void imm_mute_state_del(struct imm_mute_state const *mute)
{
    mute->super->vtable.del(mute->super);
}

static void del(struct imm_state const *state)
{
    free((void *)state->vtable.derived);
    state_del(state);
}

static imm_float lprob(struct imm_state const *state, struct imm_seq const *seq)
{
    if (imm_seq_len(seq) == 0)
        return imm_log(1.0);
    return imm_lprob_zero();
}
