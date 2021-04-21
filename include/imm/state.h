#ifndef IMM_STATE_H
#define IMM_STATE_H

#include "containers/containers.h"
#include "imm/export.h"
#include "imm/float.h"
#include "imm/lprob.h"
#include "imm/seq.h"
#include <inttypes.h>
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
    void (*destroy)(struct imm_state const* state);
    imm_float (*lprob)(struct imm_state const* state, struct imm_seq const* seq);
    uint8_t (*max_seq)(struct imm_state const* state);
    uint8_t (*min_seq)(struct imm_state const* state);
    uint8_t (*type_id)(struct imm_state const* state);
};

struct imm_state
{
    uint16_t              id;
    char const*           name;
    struct imm_abc const* abc;

    struct imm_state_vtable vtable;
    void*                   derived;
    struct stack            trans;
    struct hnode            hnode;
};

IMM_API struct imm_state*           imm_state_create(uint16_t id, char const* name, struct imm_abc const* abc,
                                                     struct imm_state_vtable vtable, void* derived);
IMM_API void                        imm_state_destroy(struct imm_state const* state);
static inline struct imm_abc const* imm_state_abc(struct imm_state const* state) { return state->abc; }
IMM_API char const*                 imm_state_name(struct imm_state const* state);
static inline uint16_t              imm_state_id(struct imm_state const* state) { return state->id; }
IMM_API imm_float                   imm_state_lprob(struct imm_state const* state, struct imm_seq const* seq);
static inline uint8_t imm_state_max_seq(struct imm_state const* state) { return state->vtable.max_seq(state); }
static inline uint8_t imm_state_min_seq(struct imm_state const* state) { return state->vtable.min_seq(state); }
static inline uint8_t imm_state_type_id(struct imm_state const* state) { return state->vtable.type_id(state); }

static inline void*       __imm_state_derived(struct imm_state const* state) { return state->derived; }
IMM_API void              __imm_state_destroy(struct imm_state const* state);
IMM_API struct imm_state* __imm_state_read(FILE* stream, struct imm_abc const* abc);
IMM_API int               __imm_state_write(struct imm_state const* state, FILE* stream);

#endif
