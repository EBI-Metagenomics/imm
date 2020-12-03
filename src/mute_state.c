#include "imm/mute_state.h"
#include "free.h"
#include "imm/lprob.h"
#include "imm/state.h"
#include "imm/state_types.h"
#include <stdlib.h>

struct imm_mute_state
{
    struct imm_state const* super;
};

static void    destroy(struct imm_state const* state);
static double  lprob(struct imm_state const* state, struct imm_seq const* seq);
static uint8_t max_seq(struct imm_state const* state);
static uint8_t min_seq(struct imm_state const* state);
static uint8_t type_id(struct imm_state const* state);

static struct imm_state_vtable const __vtable = {destroy, lprob, max_seq, min_seq, type_id};

struct imm_mute_state const* imm_mute_state_create(char const* name, struct imm_abc const* abc)
{
    struct imm_mute_state* state = malloc(sizeof(*state));

    state->super = imm_state_create(name, abc, __vtable, state);
    return state;
}

struct imm_mute_state const* imm_mute_state_derived(struct imm_state const* state)
{
    if (imm_state_type_id(state) != IMM_MUTE_STATE_TYPE_ID) {
        imm_error("could not cast to mute_state");
        return NULL;
    }
    return __imm_state_derived(state);
}

void imm_mute_state_destroy(struct imm_mute_state const* state)
{
    state->super->vtable.destroy(state->super);
}

struct imm_state const* imm_mute_state_read(FILE* stream, struct imm_abc const* abc)
{
    struct imm_state* state = __imm_state_read(stream, abc);
    if (!state) {
        imm_error("could not read normal state");
        return NULL;
    }

    struct imm_mute_state* mute_state = malloc(sizeof(*mute_state));
    mute_state->super = state;
    state->vtable = __vtable;
    state->derived = mute_state;

    return state;
}

struct imm_state const* imm_mute_state_super(struct imm_mute_state const* state)
{
    return state->super;
}

int imm_mute_state_write(struct imm_state const* state, struct imm_hmm_block const* hmm_block,
                         FILE* stream)
{
    return __imm_state_write(state, stream);
}

static void destroy(struct imm_state const* state)
{
    free_c(__imm_state_derived(state));
    __imm_state_destroy(state);
}

static double lprob(struct imm_state const* state, struct imm_seq const* seq)
{
    if (imm_seq_length(seq) == 0)
        return 0.;
    return imm_lprob_zero();
}

static uint8_t max_seq(struct imm_state const* state) { return 0; }

static uint8_t min_seq(struct imm_state const* state) { return 0; }

static uint8_t type_id(struct imm_state const* state) { return IMM_MUTE_STATE_TYPE_ID; }
