#include "imm/normal_state.h"
#include "cast.h"
#include "free.h"
#include "imm/abc.h"
#include "imm/lprob.h"
#include "imm/state.h"
#include "imm/state_types.h"
#include "normal_state.h"
#include "state.h"
#include <stdlib.h>
#include <string.h>

struct imm_normal_state
{
    struct imm_state const* base;
    double*                 lprobs;
};

struct normal_state_chunk
{
    uint8_t lprobs_size;
    double* lprobs;
};

static double   normal_state_lprob(struct imm_state const* state, struct imm_seq const* seq);
static unsigned normal_state_min_seq(struct imm_state const* state);
static unsigned normal_state_max_seq(struct imm_state const* state);
static int      normal_state_write(struct imm_state const* state, FILE* stream);
static void     normal_state_destroy(struct imm_state const* state);

struct imm_normal_state const* imm_normal_state_create(char const*           name,
                                                       struct imm_abc const* abc,
                                                       double const*         lprobs)
{
    struct imm_normal_state* state = malloc(sizeof(struct imm_normal_state));

    size_t len = imm_abc_length(abc);
    state->lprobs = malloc(sizeof(*state->lprobs) * len);
    memcpy(state->lprobs, lprobs, sizeof(*state->lprobs) * len);

    struct imm_state_funcs funcs = {normal_state_lprob, normal_state_min_seq,
                                    normal_state_max_seq, normal_state_write,
                                    normal_state_destroy};
    state->base = imm_state_create(name, abc, funcs, IMM_NORMAL_STATE_TYPE_ID, state);
    return state;
}

void imm_normal_state_destroy(struct imm_normal_state const* state)
{
    state_destroy(state->base);
    free_c(state->lprobs);
    free_c(state);
}

struct imm_state const* imm_normal_state_base(struct imm_normal_state const* state)
{
    return state->base;
}

int normal_state_read(FILE* stream, struct imm_state* state)
{
    struct normal_state_chunk chunk = {.lprobs_size = 0, .lprobs = NULL};

    uint32_t chunk_size = 0;

    if (fread(&chunk_size, sizeof(chunk_size), 1, stream) < 1) {
        imm_error("could not read chunk_size");
        return 1;
    }

    if (fread(&chunk.lprobs_size, sizeof(chunk.lprobs_size), 1, stream) < 1) {
        imm_error("could not read lprobs_size");
        return 1;
    }

    chunk.lprobs = malloc(sizeof(*chunk.lprobs) * chunk.lprobs_size);

    if (fread(chunk.lprobs, sizeof(*chunk.lprobs), chunk.lprobs_size, stream) <
        chunk.lprobs_size) {
        imm_error("could not read lprobs");
        free_c(chunk.lprobs);
        return 1;
    }

    state->lprob = normal_state_lprob;
    state->min_seq = normal_state_min_seq;
    state->max_seq = normal_state_max_seq;
    state->write = normal_state_write;
    state->destroy = normal_state_destroy;

    struct imm_normal_state* normal_state = malloc(sizeof(*normal_state));
    normal_state->base = state;
    normal_state->lprobs = chunk.lprobs;
    state->impl = normal_state;

    return 0;
}

static double normal_state_lprob(struct imm_state const* state, struct imm_seq const* seq)
{
    struct imm_normal_state const* s = imm_state_get_impl(state);
    if (imm_seq_length(seq) == 1) {
        struct imm_abc const* abc = imm_state_get_abc(state);
        unsigned              idx = imm_abc_symbol_idx(abc, imm_seq_string(seq)[0]);
        if (idx != IMM_ABC_INVALID_IDX)
            return s->lprobs[idx];
    }
    return imm_lprob_zero();
}

static unsigned normal_state_min_seq(struct imm_state const* state) { return 1; }

static unsigned normal_state_max_seq(struct imm_state const* state) { return 1; }

static int normal_state_write(struct imm_state const* state, FILE* stream)
{
    struct imm_normal_state const* s = imm_state_get_impl(state);

    struct normal_state_chunk chunk = {
        .lprobs_size = cast_u8_u(imm_abc_length(imm_state_get_abc(state))),
        .lprobs = s->lprobs,
    };

    uint32_t chunk_size =
        sizeof(chunk.lprobs_size) + sizeof(*chunk.lprobs) * chunk.lprobs_size;

    if (fwrite(&chunk_size, sizeof(chunk_size), 1, stream) < 1) {
        imm_error("could not write chunk_size");
        return 1;
    }

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

static void normal_state_destroy(struct imm_state const* state)
{
    imm_normal_state_destroy(imm_state_get_impl(state));
}
