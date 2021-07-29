#ifndef IMM_STATE_H
#define IMM_STATE_H

#include "cco/hash.h"
#include "cco/stack.h"
#include "imm/export.h"
#include "imm/float.h"
#include "imm/log.h"
#include "imm/lprob.h"
#include "imm/seq.h"
#include "imm/span.h"
#include "imm/state_types.h"
#include "imm/support.h"

struct imm_state
{
    unsigned id;
    unsigned idx;
    struct imm_abc const *abc;
    struct imm_span span;
    struct imm_state_vtable vtable;
    struct
    {
        struct cco_stack outgoing;
        struct cco_stack incoming;
    } trans;
    struct cco_hnode hnode;
    int mark;
};

typedef void (*imm_state_name)(unsigned id, char name[8]);

static inline struct imm_abc const *imm_state_abc(struct imm_state const *state)
{
    return state->abc;
}

static inline unsigned imm_state_id(struct imm_state const *state)
{
    return state->id;
}

static inline unsigned imm_state_idx(struct imm_state const *state)
{
    return state->idx;
}

static struct imm_state __imm_state_init(unsigned id, struct imm_abc const *abc,
                                         struct imm_state_vtable vtable,
                                         struct imm_span span)
{
    IMM_BUG(id >= IMM_STATE_NULL_ID);
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

static inline imm_float imm_state_lprob(struct imm_state const *state,
                                        struct imm_seq const *seq)
{
    if (imm_unlikely(state->abc != imm_seq_abc(seq)))
    {
        __imm_log(IMM_ERROR, IMM_ILLEGALARG, "alphabets must be the same");
        return IMM_LPROB_NAN;
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
