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

typedef uint8_t (*imm_state_type_id_t)(struct imm_state const* state);
typedef double (*imm_state_lprob_t)(struct imm_state const* state, struct imm_seq const* seq);
typedef unsigned (*imm_state_min_seq_t)(struct imm_state const* state);
typedef unsigned (*imm_state_max_seq_t)(struct imm_state const* state);
typedef int (*imm_state_write_t)(struct imm_state const* state, FILE* stream);
typedef void (*imm_state_destroy_t)(struct imm_state const* state);

struct imm_state_vtable
{
    imm_state_type_id_t type_id;
    imm_state_lprob_t   lprob;
    imm_state_min_seq_t min_seq;
    imm_state_max_seq_t max_seq;
    imm_state_write_t   write;
    imm_state_destroy_t destroy;
};

struct imm_state
{
    char const*           name;
    struct imm_abc const* abc;

    struct imm_state_vtable vtable;
    void*                   child;
};

IMM_EXPORT struct imm_state const* imm_state_create(char const* name, struct imm_abc const* abc,
                                                    struct imm_state_vtable vtable, void* child);
IMM_EXPORT void                    imm_state_destroy(struct imm_state const* state);
IMM_EXPORT void                    imm_state_destroy_parent(struct imm_state const* state);

static inline char const* imm_state_get_name(struct imm_state const* state) { return state->name; }

static inline struct imm_abc const* imm_state_get_abc(struct imm_state const* state)
{
    return state->abc;
}

static inline uint8_t imm_state_type_id(struct imm_state const* state)
{
    return state->vtable.type_id(state);
}

static inline double imm_state_lprob(struct imm_state const* state, struct imm_seq const* seq)
{
    if (state->abc != imm_seq_get_abc(seq)) {
        imm_error("alphabets must be the same");
        return imm_lprob_invalid();
    }
    return state->vtable.lprob(state, seq);
}

static inline unsigned imm_state_min_seq(struct imm_state const* state)
{
    return state->vtable.min_seq(state);
}

static inline unsigned imm_state_max_seq(struct imm_state const* state)
{
    return state->vtable.max_seq(state);
}

static inline void const* __imm_state_child(struct imm_state const* state) { return state->child; }

#endif
