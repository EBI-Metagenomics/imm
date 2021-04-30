#ifndef IMM_STATE_H
#define IMM_STATE_H

#include "containers/hash.h"
#include "containers/stack.h"
#include "imm/export.h"
#include "imm/float.h"
#include "imm/log.h"
#include "imm/lprob.h"
#include "imm/seq.h"
#include "imm/state_types.h"
#include <stdio.h>

struct imm_abc;
struct imm_state;

struct imm_state_vtable
{
    void (*del)(struct imm_state const *state);
    imm_float (*lprob)(struct imm_state const *state,
                       struct imm_seq const *seq);
    imm_state_tid_t typeid;
    void *derived;
};

struct imm_state
{
    imm_state_id_t id;
    imm_state_idx_t idx;
    struct imm_abc const *abc;
    struct
    {
        unsigned max;
        unsigned min;
    } seqlen;
    struct imm_state_vtable vtable;
    struct
    {
        struct stack outgoing;
        struct stack incoming;
    } trans;
    struct hnode hnode;
};

static inline struct imm_abc const *imm_state_abc(struct imm_state const *state)
{
    return state->abc;
}

static inline void imm_state_del(struct imm_state const *state)
{
    state->vtable.del(state);
}

static inline imm_state_id_t imm_state_id(struct imm_state const *state)
{
    return state->id;
}

static inline imm_float imm_state_lprob(struct imm_state const *state,
                                        struct imm_seq const *seq)
{
    if (state->abc != imm_seq_abc(seq))
    {
        imm_log_error("alphabets must be the same");
        return imm_lprob_nan();
    }
    return state->vtable.lprob(state, seq);
}

static inline unsigned imm_state_max_seqlen(struct imm_state const *state)
{
    return state->seqlen.max;
}

static inline unsigned imm_state_min_seqlen(struct imm_state const *state)
{
    return state->seqlen.min;
}

static inline imm_state_tid_t imm_state_typeid(struct imm_state const *state)
{
    return state->vtable.typeid;
}

#endif
