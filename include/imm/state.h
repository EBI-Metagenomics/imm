#ifndef IMM_STATE_H
#define IMM_STATE_H

#include "containers/hash.h"
#include "containers/stack.h"
#include "imm/export.h"
#include "imm/float.h"
#include "imm/log.h"
#include "imm/lprob.h"
#include "imm/seq.h"
#include "imm/span.h"
#include "imm/state_types.h"

struct imm_state
{
    unsigned id;
    unsigned idx;
    struct imm_abc const *abc;
    struct imm_span span;
    struct imm_state_vtable vtable;
    struct
    {
        struct stack outgoing;
        struct stack incoming;
    } trans;
    struct hnode hnode;
    int mark;
};

static inline struct imm_abc const *imm_state_abc(struct imm_state const *state)
{
    return state->abc;
}

static inline void imm_state_del(struct imm_state const *state)
{
    state->vtable.del(state);
}

static inline unsigned imm_state_id(struct imm_state const *state)
{
    return state->id;
}

static inline imm_float imm_state_lprob(struct imm_state const *state,
                                        struct imm_seq const *seq)
{
    if (state->abc != imm_seq_abc(seq))
    {
        __imm_log(IMM_ERROR, IMM_ILLEGALARG, "alphabets must be the same");
        return imm_lprob_nan();
    }
    return state->vtable.lprob(state, seq);
}

static inline struct imm_span imm_state_span(struct imm_state const *state)
{
    return state->span;
}

static inline enum imm_state_typeid
imm_state_typeid(struct imm_state const *state)
{
    return state->vtable.typeid;
}

#endif
