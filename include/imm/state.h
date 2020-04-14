#ifndef IMM_STATE_H
#define IMM_STATE_H

#include "imm/export.h"
#include "imm/lprob.h"
#include "imm/report.h"
#include "imm/seq.h"
#include <inttypes.h>
#include <stdio.h>

/** @file state.h
 * State module.
 *
 * An object of type @ref imm_state is meant to be immutable.
 */

struct imm_abc;
struct imm_state;

typedef double (*imm_state_lprob_t)(struct imm_state const* state, struct imm_seq const* seq);
typedef unsigned (*imm_state_min_seq_t)(struct imm_state const* state);
typedef unsigned (*imm_state_max_seq_t)(struct imm_state const* state);
typedef int (*imm_state_write_t)(struct imm_state const* state, FILE* stream);

struct imm_state_funcs
{
    imm_state_lprob_t   lprob;
    imm_state_min_seq_t min_seq;
    imm_state_max_seq_t max_seq;
    imm_state_write_t   write;
};

struct imm_state
{
    char const*           name;
    struct imm_abc const* abc;

    imm_state_lprob_t   lprob;
    imm_state_min_seq_t min_seq;
    imm_state_max_seq_t max_seq;
    imm_state_write_t   write;
    uint8_t             type_id;
    void*               impl;
};

IMM_EXPORT struct imm_state const* imm_state_create(char const*            name,
                                                    struct imm_abc const*  abc,
                                                    struct imm_state_funcs funcs,
                                                    uint8_t type_id, void* impl);
IMM_EXPORT void                    imm_state_destroy(struct imm_state const* state);

static inline char const* imm_state_get_name(struct imm_state const* state)
{
    return state->name;
}

static inline struct imm_abc const* imm_state_get_abc(struct imm_state const* state)
{
    return state->abc;
}

static inline double imm_state_lprob(struct imm_state const* state, struct imm_seq const* seq)
{
    if (state->abc != imm_seq_get_abc(seq)) {
        imm_error("alphabets must be the same");
        return imm_lprob_invalid();
    }
    return state->lprob(state, seq);
}
static inline unsigned imm_state_min_seq(struct imm_state const* state)
{
    return state->min_seq(state);
}
static inline unsigned imm_state_max_seq(struct imm_state const* state)
{
    return state->max_seq(state);
}
static inline int imm_state_write(struct imm_state const* state, FILE* stream)
{
    return state->write(state, stream);
}
static inline uint8_t imm_state_type_id(struct imm_state const* state)
{
    return state->type_id;
}

static inline struct imm_state const* imm_state_cast_c(void const* state)
{
    struct type_c
    {
        struct imm_state const* state;
    } const* t = state;
    return t->state;
}

static inline void const* imm_state_get_impl_c(struct imm_state const* state)
{
    return state->impl;
}

#endif
