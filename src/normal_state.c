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
    struct imm_state const* parent;
    double*                 lprobs;
};

struct normal_state_chunk
{
    uint8_t lprobs_size;
    double* lprobs;
};

static uint8_t  normal_state_type_id(struct imm_state const* state);
static double   normal_state_lprob(struct imm_state const* state, struct imm_seq const* seq);
static unsigned normal_state_min_seq(struct imm_state const* state);
static unsigned normal_state_max_seq(struct imm_state const* state);
static int      normal_state_write(struct imm_state const* state, FILE* stream);
static void     normal_state_destroy(struct imm_state const* state);

static struct imm_state_vtable const vtable = {normal_state_type_id, normal_state_lprob,
                                               normal_state_min_seq, normal_state_max_seq,
                                               normal_state_write,   normal_state_destroy};

struct imm_normal_state const* imm_normal_state_create(char const* name, struct imm_abc const* abc,
                                                       double const* lprobs)
{
    struct imm_normal_state* state = malloc(sizeof(struct imm_normal_state));

    size_t len = imm_abc_length(abc);
    state->lprobs = malloc(sizeof(*state->lprobs) * len);
    memcpy(state->lprobs, lprobs, sizeof(*state->lprobs) * len);

    state->parent = imm_state_create(name, abc, vtable, state);
    return state;
}

void imm_normal_state_destroy(struct imm_normal_state const* state)
{
    struct imm_state const* parent = state->parent;
    normal_state_destroy(parent);
    imm_state_destroy_parent(parent);
}

struct imm_state const* imm_normal_state_parent(struct imm_normal_state const* state)
{
    return state->parent;
}

struct imm_normal_state const* imm_normal_state_child(struct imm_state const* state)
{
    if (imm_state_type_id(state) != IMM_NORMAL_STATE_TYPE_ID) {
        imm_error("could not cast to normal_state");
        return NULL;
    }
    return __imm_state_child(state);
}

static uint8_t normal_state_type_id(struct imm_state const* state)
{
    return IMM_NORMAL_STATE_TYPE_ID;
}

static double normal_state_lprob(struct imm_state const* state, struct imm_seq const* seq)
{
    struct imm_normal_state const* s = __imm_state_child(state);
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
    if (state_write_parent(state, stream))
        return 1;

    struct imm_normal_state const* s = __imm_state_child(state);

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

static void normal_state_destroy(struct imm_state const* state)
{
    struct imm_normal_state const* s = __imm_state_child(state);
    free_c(s->lprobs);
    free_c(s);
}

struct imm_state const* normal_state_read(FILE* stream, struct imm_abc const* abc)
{
    struct imm_state* state = state_read_parent(stream, abc);
    if (!state) {
        imm_error("could not state_read_parent");
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
    normal_state->parent = state;
    normal_state->lprobs = chunk.lprobs;
    state->child = normal_state;

    return state;
}
