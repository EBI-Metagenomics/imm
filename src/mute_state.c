#include "mute_state.h"
#include "free.h"
#include "imm/lprob.h"
#include "imm/mute_state.h"
#include "imm/state.h"
#include "imm/state_types.h"
#include "state.h"
#include <stdlib.h>

struct imm_mute_state
{
    struct imm_state const* parent;
};

static uint8_t  mute_state_type_id(struct imm_state const* state);
static double   mute_state_lprob(struct imm_state const* state, struct imm_seq const* seq);
static unsigned mute_state_min_seq(struct imm_state const* state);
static unsigned mute_state_max_seq(struct imm_state const* state);
static int  mute_state_write(struct imm_state const* state, struct imm_io const* io, FILE* stream);
static void mute_state_destroy(struct imm_state const* state);

static struct imm_state_vtable const vtable = {mute_state_type_id, mute_state_lprob,
                                               mute_state_min_seq, mute_state_max_seq,
                                               mute_state_write,   mute_state_destroy};

struct imm_mute_state const* imm_mute_state_create(char const* name, struct imm_abc const* abc)
{
    struct imm_mute_state* state = malloc(sizeof(struct imm_mute_state));

    state->parent = imm_state_create(name, abc, vtable, state);
    return state;
}

void imm_mute_state_destroy(struct imm_mute_state const* state)
{
    struct imm_state const* parent = state->parent;
    mute_state_destroy(parent);
    __imm_state_destroy_parent(parent);
}

struct imm_state const* imm_mute_state_parent(struct imm_mute_state const* state)
{
    return state->parent;
}

struct imm_mute_state const* imm_mute_state_child(struct imm_state const* state)
{
    if (imm_state_type_id(state) != IMM_MUTE_STATE_TYPE_ID) {
        imm_error("could not cast to mute_state");
        return NULL;
    }
    return __imm_state_child(state);
}

static uint8_t mute_state_type_id(struct imm_state const* state) { return IMM_MUTE_STATE_TYPE_ID; }

static double mute_state_lprob(struct imm_state const* state, struct imm_seq const* seq)
{
    if (imm_seq_length(seq) == 0)
        return 0.0;
    return imm_lprob_zero();
}

static unsigned mute_state_min_seq(struct imm_state const* state) { return 0; }

static unsigned mute_state_max_seq(struct imm_state const* state) { return 0; }

static int mute_state_write(struct imm_state const* state, struct imm_io const* io, FILE* stream)
{
    if (state_write_parent(state, stream))
        return 1;

    return 0;
}

struct imm_state const* mute_state_read(FILE* stream, struct imm_abc const* abc)
{
    struct imm_state* state = state_read_parent(stream, abc);
    if (!state) {
        imm_error("could not state_read_parent");
        return NULL;
    }

    state->vtable = vtable;

    struct imm_mute_state* mute_state = malloc(sizeof(*mute_state));
    mute_state->parent = state;
    state->child = mute_state;

    return state;
}

static void mute_state_destroy(struct imm_state const* state)
{
    struct imm_mute_state const* s = __imm_state_child(state);
    free_c(s);
}
