#include "imm/mute_state.h"
#include "state.h"
#include "support.h"

static void del(struct imm_state const *state);

static imm_float lprob(struct imm_state const *state,
                       struct imm_seq const *seq);

struct imm_mute_state *imm_mute_state_new(unsigned id,
                                          struct imm_abc const *abc)
{
    struct imm_mute_state *mute = xmalloc(sizeof(*mute));
    struct imm_state_vtable vtable = {del, lprob, IMM_MUTE_STATE, mute};
    mute->super = state_init(id, abc, vtable, IMM_SPAN(0, 0));
    return mute;
}

static void del(struct imm_state const *state)
{
    if (state)
        free((void *)state->vtable.derived);
}

static imm_float lprob(struct imm_state const *state, struct imm_seq const *seq)
{
    if (imm_seq_size(seq) == 0)
        return imm_log(1.0);
    return imm_lprob_zero();
}
