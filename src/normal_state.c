#include "normal_state.h"
#include "cast.h"
#include "free.h"
#include "imm/abc.h"
#include "imm/lprob.h"
#include "imm/normal_state.h"
#include "imm/state.h"
#include "imm/state_types.h"
#include "state.h"
#include <stdlib.h>
#include <string.h>

struct imm_normal_state
{
    struct imm_state const* super;
    double*                 lprobs;
};

struct normal_state_chunk
{
    uint8_t lprobs_size;
    double* lprobs;
};

static uint8_t  type_id(struct imm_state const* state);
static double   lprob(struct imm_state const* state, struct imm_seq const* seq);
static unsigned min_seq(struct imm_state const* state);
static unsigned max_seq(struct imm_state const* state);
static int      write(struct imm_state const* state, struct imm_io const* io, FILE* stream);
static void     destroy(struct imm_state const* state);

static struct imm_state_vtable const vtable = {type_id, lprob, min_seq, max_seq, write, destroy};

struct imm_normal_state const* imm_normal_state_create(char const* name, struct imm_abc const* abc,
                                                       double const* lprobs)
{
    struct imm_normal_state* state = malloc(sizeof(struct imm_normal_state));

    size_t len = imm_abc_length(abc);
    state->lprobs = malloc(sizeof(*state->lprobs) * len);
    memcpy(state->lprobs, lprobs, sizeof(*state->lprobs) * len);

    state->super = imm_state_create(name, abc, vtable, state);
    return state;
}

void imm_normal_state_destroy(struct imm_normal_state const* state)
{
    state->super->vtable.destroy(state->super);
}

struct imm_state const* imm_normal_state_super(struct imm_normal_state const* state)
{
    return state->super;
}

struct imm_normal_state const* imm_normal_state_derived(struct imm_state const* state)
{
    if (imm_state_type_id(state) != IMM_NORMAL_STATE_TYPE_ID) {
        imm_error("could not cast to normal_state");
        return NULL;
    }
    return __imm_state_derived(state);
}

static uint8_t type_id(struct imm_state const* state) { return IMM_NORMAL_STATE_TYPE_ID; }

static double lprob(struct imm_state const* state, struct imm_seq const* seq)
{
    struct imm_normal_state const* s = __imm_state_derived(state);
    if (imm_seq_length(seq) == 1) {
        struct imm_abc const* abc = imm_state_get_abc(state);
        unsigned              idx = imm_abc_symbol_idx(abc, imm_seq_string(seq)[0]);
        if (idx != IMM_ABC_INVALID_IDX)
            return s->lprobs[idx];
    }
    return imm_lprob_zero();
}

static unsigned min_seq(struct imm_state const* state) { return 1; }

static unsigned max_seq(struct imm_state const* state) { return 1; }

static int write(struct imm_state const* state, struct imm_io const* io, FILE* stream)
{
    if (__imm_state_write(state, stream))
        return 1;

    struct imm_normal_state const* s = __imm_state_derived(state);

    struct normal_state_chunk chunk = {
        .lprobs_size = cast_u8_u(imm_abc_length(imm_state_get_abc(state))),
        .lprobs = s->lprobs,
    };

    if (fwrite(&chunk.lprobs_size, sizeof(chunk.lprobs_size), 1, stream) < 1) {
        imm_error("could not write lprobs_size");
        return 1;
    }

    if (fwrite(chunk.lprobs, sizeof(*chunk.lprobs), chunk.lprobs_size, stream) <
        chunk.lprobs_size) {
        imm_error("could not write lprobs");
        return 1;
    }

    return 0;
}

static void destroy(struct imm_state const* state)
{
    struct imm_normal_state const* s = __imm_state_derived(state);
    free_c(s->lprobs);
    free_c(s);
    __imm_state_destroy(state);
}

struct imm_state const* normal_state_read(FILE* stream, struct imm_abc const* abc)
{
    struct imm_state* state = __imm_state_read(stream, abc);
    if (!state) {
        imm_error("could not state_read");
        return NULL;
    }

    struct normal_state_chunk chunk = {.lprobs_size = 0, .lprobs = NULL};

    if (fread(&chunk.lprobs_size, sizeof(chunk.lprobs_size), 1, stream) < 1) {
        imm_error("could not read lprobs_size");
        imm_state_destroy(state);
        return NULL;
    }

    chunk.lprobs = malloc(sizeof(*chunk.lprobs) * chunk.lprobs_size);

    if (fread(chunk.lprobs, sizeof(*chunk.lprobs), chunk.lprobs_size, stream) < chunk.lprobs_size) {
        imm_error("could not read lprobs");
        free_c(chunk.lprobs);
        imm_state_destroy(state);
        return NULL;
    }

    state->vtable = vtable;

    struct imm_normal_state* normal_state = malloc(sizeof(*normal_state));
    normal_state->super = state;
    normal_state->lprobs = chunk.lprobs;
    state->derived = normal_state;

    return state;
}
