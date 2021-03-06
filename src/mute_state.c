#include "imm/imm.h"
#include "std.h"
#include <stdlib.h>

struct imm_mute_state
{
    struct imm_state* super;
};

static void      destroy(struct imm_state const* state);
static imm_float lprob(struct imm_state const* state, struct imm_seq const* seq);
static uint8_t   max_seq(struct imm_state const* state) { return 0; }
static uint8_t   min_seq(struct imm_state const* state) { return 0; }
static uint8_t   type_id(struct imm_state const* state) { return IMM_MUTE_STATE_TYPE_ID; }

static struct imm_state_vtable const __vtable = {destroy, lprob, max_seq, min_seq, type_id};

struct imm_mute_state* imm_mute_state_create(uint16_t id, char const* name, struct imm_abc const* abc)
{
    struct imm_mute_state* state = xmalloc(sizeof(*state));

    state->super = imm_state_create(id, name, abc, __vtable, state);
    return state;
}

struct imm_mute_state* imm_mute_state_derived(struct imm_state const* state)
{
    if (imm_state_type_id(state) != IMM_MUTE_STATE_TYPE_ID) {
        error("could not cast to mute_state");
        return NULL;
    }
    return __imm_state_derived(state);
}

void imm_mute_state_destroy(struct imm_mute_state const* state) { state->super->vtable.destroy(state->super); }

struct imm_state* imm_mute_state_read(FILE* stream, struct imm_abc const* abc)
{
    struct imm_state* state = __imm_state_read(stream, abc);
    if (!state) {
        error("could not read normal state");
        return NULL;
    }

    struct imm_mute_state* mute_state = xmalloc(sizeof(*mute_state));
    mute_state->super = state;
    state->vtable = __vtable;
    state->derived = mute_state;

    return state;
}

struct imm_state* imm_mute_state_super(struct imm_mute_state const* state) { return state->super; }

int imm_mute_state_write(struct imm_state const* state, struct imm_profile const* prof, FILE* stream)
{
    return __imm_state_write(state, stream);
}

static void destroy(struct imm_state const* state)
{
    free((void*)__imm_state_derived(state));
    __imm_state_destroy(state);
}

static imm_float lprob(struct imm_state const* state, struct imm_seq const* seq)
{
    if (imm_seq_length(seq) == 0)
        return (imm_float)0.;
    return imm_lprob_zero();
}
