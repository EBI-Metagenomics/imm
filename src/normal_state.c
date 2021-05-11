#include "imm/normal_state.h"
#include "common/common.h"
#include "imm/abc.h"
#include "state.h"

static void del(struct imm_state const *state);

static imm_float lprob(struct imm_state const *state,
                       struct imm_seq const *seq);

struct imm_normal_state *imm_normal_state_new(unsigned id,
                                              struct imm_abc const *abc,
                                              imm_float const lprobs[static 1])
{
    struct imm_normal_state *normal = xmalloc(sizeof(*normal));
    normal->lprobs = lprobs;
    struct imm_state_vtable vtable = {del, lprob, IMM_NORMAL_STATE, normal};
    normal->super = state_new(id, abc, vtable, IMM_SPAN(1, 1));
    return normal;
}

static void del(struct imm_state const *state)
{
    struct imm_normal_state const *normal = state->vtable.derived;
    free((void *)normal);
    state_del(state);
}

static imm_float lprob(struct imm_state const *state, struct imm_seq const *seq)
{
    struct imm_normal_state const *normal = state->vtable.derived;
    if (imm_seq_len(seq) == 1)
    {
        struct imm_abc const *abc = imm_state_abc(state);
        unsigned idx = imm_abc_symbol_idx(abc, seq->str[0]);
        if (idx != IMM_SYM_NULL_IDX)
            return normal->lprobs[idx];
    }
    return imm_lprob_zero();
}
