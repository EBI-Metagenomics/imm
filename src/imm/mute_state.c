#include "imm.h"
#include "src/imm/hide.h"
#include <math.h>
#include <stdlib.h>

struct imm_mute_state
{
    struct imm_state *interface;
};

HIDE double mute_state_lprob(const struct imm_state *state, const char *seq, int seq_len);
HIDE int mute_state_min_seq(const struct imm_state *state);
HIDE int mute_state_max_seq(const struct imm_state *state);

struct imm_mute_state *imm_mute_state_create(const char *name, const struct imm_abc *abc)
{
    struct imm_mute_state *state = malloc(sizeof(struct imm_mute_state));

    struct imm_state_funcs funcs = {mute_state_lprob, mute_state_min_seq, mute_state_max_seq};
    state->interface = imm_state_create(name, abc, funcs, state);
    return state;
}

struct imm_state *imm_mute_state_cast(struct imm_mute_state *state)
{
    return state->interface;
}

const struct imm_state *imm_mute_state_cast_c(const struct imm_mute_state *state)
{
    return state->interface;
}

void imm_mute_state_destroy(struct imm_mute_state *state)
{
    if (!state)
        return;

    imm_state_destroy(state->interface);
    state->interface = NULL;
}

double mute_state_lprob(const struct imm_state *state, const char *seq, int seq_len)
{
    if (seq_len == 0)
        return 0.0;
    return -INFINITY;
}

int mute_state_min_seq(const struct imm_state *state) { return 0; }

int mute_state_max_seq(const struct imm_state *state) { return 0; }
