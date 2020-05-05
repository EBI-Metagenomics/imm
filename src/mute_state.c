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
    struct imm_state const* super;
};

static uint8_t type_id(struct imm_state const* state);
static double  lprob(struct imm_state const* state, struct imm_seq const* seq);
static uint8_t min_seq(struct imm_state const* state);
static uint8_t max_seq(struct imm_state const* state);
static int     write(struct imm_state const* state, struct imm_io const* io, FILE* stream);
static void    destroy(struct imm_state const* state);

static struct imm_state_vtable const vtable = {type_id, lprob, min_seq, max_seq, write, destroy};

struct imm_mute_state const* imm_mute_state_create(char const* name, struct imm_abc const* abc)
{
    struct imm_mute_state* state = malloc(sizeof(struct imm_mute_state));

    state->super = imm_state_create(name, abc, vtable, state);
    return state;
}

void imm_mute_state_destroy(struct imm_mute_state const* state)
{
    state->super->vtable.destroy(state->super);
}

struct imm_state const* imm_mute_state_super(struct imm_mute_state const* state)
{
    return state->super;
}

struct imm_mute_state const* imm_mute_state_derived(struct imm_state const* state)
{
    if (imm_state_type_id(state) != IMM_MUTE_STATE_TYPE_ID) {
        imm_error("could not cast to mute_state");
        return NULL;
    }
    return __imm_state_derived(state);
}

static uint8_t type_id(struct imm_state const* state) { return IMM_MUTE_STATE_TYPE_ID; }

static double lprob(struct imm_state const* state, struct imm_seq const* seq)
{
    if (imm_seq_length(seq) == 0)
        return 0.0;
    return imm_lprob_zero();
}

static uint8_t min_seq(struct imm_state const* state) { return 0; }

static uint8_t max_seq(struct imm_state const* state) { return 0; }

static int write(struct imm_state const* state, struct imm_io const* io, FILE* stream)
{
    if (__imm_state_write(state, stream))
        return 1;

    return 0;
}

struct imm_state const* mute_state_read(FILE* stream, struct imm_abc const* abc)
{
    struct imm_state* state = __imm_state_read(stream, abc);
    if (!state) {
        imm_error("could not state_read");
        return NULL;
    }

    state->vtable = vtable;

    struct imm_mute_state* mute_state = malloc(sizeof(*mute_state));
    mute_state->super = state;
    state->derived = mute_state;

    return state;
}

static void destroy(struct imm_state const* state)
{
    struct imm_mute_state const* s = __imm_state_derived(state);
    free_c(s);
    __imm_state_destroy(state);
}
