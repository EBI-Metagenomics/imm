#include "imm/imm.h"
#include "std.h"
#include <stdlib.h>
#include <string.h>

struct imm_normal_state
{
    struct imm_state* super;
    imm_float*        lprobs;
};

struct normal_state_chunk
{
    uint8_t    lprobs_size;
    imm_float* lprobs;
};

static void      destroy(struct imm_state const* state);
static imm_float lprob(struct imm_state const* state, struct imm_seq const* seq);
static uint8_t   min_seq(struct imm_state const* state) { return 1; }
static uint8_t   max_seq(struct imm_state const* state) { return 1; }
static uint8_t   type_id(struct imm_state const* state) { return IMM_NORMAL_STATE_TYPE_ID; }

static struct imm_state_vtable const __vtable = {destroy, lprob, max_seq, min_seq, type_id};

struct imm_normal_state* imm_normal_state_create(uint16_t id, char const* name, struct imm_abc const* abc,
                                                 imm_float const* lprobs)
{
    struct imm_normal_state* state = xmalloc(sizeof(*state));

    size_t len = imm_abc_length(abc);
    state->lprobs = xmalloc(sizeof(*state->lprobs) * len);
    BUG(!memcpy(state->lprobs, lprobs, sizeof(*state->lprobs) * len));

    state->super = imm_state_create(id, name, abc, __vtable, state);
    return state;
}

struct imm_normal_state* imm_normal_state_derived(struct imm_state const* state)
{
    if (imm_state_type_id(state) != IMM_NORMAL_STATE_TYPE_ID) {
        error("could not cast to normal_state");
        return NULL;
    }
    return __imm_state_derived(state);
}

void imm_normal_state_destroy(struct imm_normal_state const* state) { state->super->vtable.destroy(state->super); }

struct imm_state* imm_normal_state_read(FILE* stream, struct imm_abc const* abc)
{
    struct imm_state* state = __imm_state_read(stream, abc);
    if (!state) {
        error("could not read normal state");
        return NULL;
    }

    struct normal_state_chunk chunk = {.lprobs_size = 0, .lprobs = NULL};

    if (fread(&chunk.lprobs_size, sizeof(chunk.lprobs_size), 1, stream) < 1) {
        error("could not read lprobs_size");
        imm_state_destroy(state);
        return NULL;
    }

    chunk.lprobs = xmalloc(sizeof(*chunk.lprobs) * chunk.lprobs_size);

    if (fread(chunk.lprobs, sizeof(*chunk.lprobs), chunk.lprobs_size, stream) < chunk.lprobs_size) {
        error("could not read lprobs");
        free(chunk.lprobs);
        imm_state_destroy(state);
        return NULL;
    }

    struct imm_normal_state* normal_state = xmalloc(sizeof(*normal_state));
    normal_state->super = state;
    normal_state->lprobs = chunk.lprobs;
    state->vtable = __vtable;
    state->derived = normal_state;

    return state;
}

struct imm_state* imm_normal_state_super(struct imm_normal_state const* state) { return state->super; }

int imm_normal_state_write(struct imm_state const* state, struct imm_profile const* prof, FILE* stream)
{
    if (__imm_state_write(state, stream))
        return 1;

    struct imm_normal_state const* s = __imm_state_derived(state);

    struct normal_state_chunk chunk = {
        .lprobs_size = imm_abc_length(imm_state_abc(state)),
        .lprobs = s->lprobs,
    };

    if (fwrite(&chunk.lprobs_size, sizeof(chunk.lprobs_size), 1, stream) < 1) {
        error("could not write lprobs_size");
        return 1;
    }

    if (fwrite(chunk.lprobs, sizeof(*chunk.lprobs), chunk.lprobs_size, stream) < chunk.lprobs_size) {
        error("could not write lprobs");
        return 1;
    }

    return 0;
}

static void destroy(struct imm_state const* state)
{
    struct imm_normal_state const* s = __imm_state_derived(state);
    free(s->lprobs);
    free((void*)s);
    __imm_state_destroy(state);
}

static imm_float lprob(struct imm_state const* state, struct imm_seq const* seq)
{
    struct imm_normal_state const* s = __imm_state_derived(state);
    if (imm_seq_length(seq) == 1) {
        struct imm_abc const* abc = imm_state_abc(state);
        unsigned              idx = imm_abc_symbol_idx(abc, imm_seq_string(seq)[0]);
        if (idx != IMM_ABC_INVALID_IDX)
            return s->lprobs[idx];
    }
    return imm_lprob_zero();
}
