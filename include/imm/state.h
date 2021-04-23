#ifndef IMM_STATE_H
#define IMM_STATE_H

#include "containers/hash.h"
#include "containers/stack.h"
#include "imm/export.h"
#include "imm/float.h"
#include "imm/log.h"
#include "imm/lprob.h"
#include "imm/seq.h"
#include <stdint.h>
#include <stdio.h>

/** @file state.h
 * State module.
 *
 * An object of type @ref imm_state is meant to be immutable.
 */

struct imm_abc;
struct imm_profile;
struct imm_state;

struct imm_state_vtable
{
    void (*del)(struct imm_state const *state);
    imm_float (*lprob)(struct imm_state const *state,
                       struct imm_seq const *seq);
    uint8_t (*max_seq)(struct imm_state const *state);
    uint8_t (*min_seq)(struct imm_state const *state);
    uint8_t (*type_id)(struct imm_state const *state);
};

struct imm_state
{
    uint16_t id;
    struct imm_abc const *abc;
    struct imm_state_vtable vtable;
    void *derived;
    struct stack trans;
    struct hnode hnode;
};

static inline struct imm_abc const *imm_state_abc(struct imm_state const *state)
{
    return state->abc;
}

IMM_API struct imm_state *imm_state_new(uint16_t id, struct imm_abc const *abc,
                                        struct imm_state_vtable vtable,
                                        void *derived);

static inline void imm_state_free(struct imm_state const *state)
{
    state->vtable.del(state);
}

static inline uint16_t imm_state_id(struct imm_state const *state)
{
    return state->id;
}

static inline imm_float imm_state_lprob(struct imm_state const *state,
                                        struct imm_seq const *seq)
{
    if (state->abc != imm_seq_abc(seq))
    {
        imm_log_error("alphabets must be the same");
        return imm_lprob_invalid();
    }
    return state->vtable.lprob(state, seq);
}

static inline uint8_t imm_state_max_seq(struct imm_state const *state)
{
    return state->vtable.max_seq(state);
}

static inline uint8_t imm_state_min_seq(struct imm_state const *state)
{
    return state->vtable.min_seq(state);
}

static inline uint8_t imm_state_type_id(struct imm_state const *state)
{
    return state->vtable.type_id(state);
}

static inline void *__imm_state_derived(struct imm_state const *state)
{
    return state->derived;
}

static inline void __imm_state_free(struct imm_state const *state)
{
    free((void *)state);
}

IMM_API struct imm_state *__imm_state_read(FILE *stream,
                                           struct imm_abc const *abc);

IMM_API void __imm_state_write(struct imm_state const *state, FILE *stream);

#endif
