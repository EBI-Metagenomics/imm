#include "state.h"
#include "imm/mute_state.h"
#include "free.h"
#include "imm/lprob.h"
#include "imm/state.h"
#include "imm/state_types.h"
#include "mute_state.h"
#include <stdlib.h>

struct imm_mute_state
{
    struct imm_state const* interface;
};

static double   mute_state_lprob(struct imm_state const* state, struct imm_seq const* seq);
static unsigned mute_state_min_seq(struct imm_state const* state);
static unsigned mute_state_max_seq(struct imm_state const* state);
static int      mute_state_write(struct imm_state const* state, FILE* stream);
static void     mute_state_destroy(struct imm_state const* state);

struct imm_mute_state const* imm_mute_state_create(char const* name, struct imm_abc const* abc)
{
    struct imm_mute_state* state = malloc(sizeof(struct imm_mute_state));

    struct imm_state_funcs funcs = {mute_state_lprob, mute_state_min_seq, mute_state_max_seq,
                                    mute_state_write, mute_state_destroy};
    state->interface = imm_state_create(name, abc, funcs, IMM_MUTE_STATE_TYPE_ID, state);
    return state;
}

void imm_mute_state_destroy(struct imm_mute_state const* state)
{
    state_destroy(state->interface);
    free_c(state);
}

int mute_state_read(FILE* stream, struct imm_state* state)
{
    uint32_t chunk_size = 0;

    if (fread(&chunk_size, sizeof(chunk_size), 1, stream) < 1) {
        imm_error("could not read chunk_size");
        return 1;
    }

    if (chunk_size != 0) {
        imm_error("invalid chunk_size");
        return 1;
    }

    state->lprob = mute_state_lprob;
    state->min_seq = mute_state_min_seq;
    state->max_seq = mute_state_max_seq;
    state->write = mute_state_write;
    state->destroy = mute_state_destroy;

    struct imm_mute_state* mute_state = malloc(sizeof(*mute_state));
    mute_state->interface = state;
    state->impl = mute_state;

    return 0;
}

static double mute_state_lprob(struct imm_state const* state, struct imm_seq const* seq)
{
    if (imm_seq_length(seq) == 0)
        return 0.0;
    return imm_lprob_zero();
}

static unsigned mute_state_min_seq(struct imm_state const* state) { return 0; }

static unsigned mute_state_max_seq(struct imm_state const* state) { return 0; }

static int mute_state_write(struct imm_state const* state, FILE* stream)
{
    uint32_t chunk_size = 0;

    if (fwrite(&chunk_size, sizeof(chunk_size), 1, stream) < 1) {
        imm_error("could not write chunk_size");
        return 1;
    }

    return 0;
}

static void mute_state_destroy(struct imm_state const* state)
{
    imm_mute_state_destroy(imm_state_get_impl(state));
}
