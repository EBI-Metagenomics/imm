#include "free.h"
#include "imm/imm.h"
#include <math.h>
#include <stdlib.h>

struct imm_mute_state
{
    struct imm_state const* interface;
};

static double   mute_state_lprob(struct imm_state const* state, char const* seq,
                                 unsigned seq_len);
static unsigned mute_state_min_seq(struct imm_state const* state);
static unsigned mute_state_max_seq(struct imm_state const* state);

struct imm_mute_state const* imm_mute_state_create(char const* name, struct imm_abc const* abc)
{
    struct imm_mute_state* state = malloc(sizeof(struct imm_mute_state));

    struct imm_state_funcs funcs = {mute_state_lprob, mute_state_min_seq, mute_state_max_seq};
    state->interface = imm_state_create(name, abc, funcs, state);
    return state;
}

void imm_mute_state_destroy(struct imm_mute_state const* state)
{
    if (!state) {
        imm_error("state should not be NULL");
        return;
    }

    imm_state_destroy(state->interface);
    free_c(state);
}

static double mute_state_lprob(struct imm_state const* state, char const* seq,
                               unsigned seq_len)
{
    if (seq_len == 0)
        return 0.0;
    return imm_lprob_zero();
}

static unsigned mute_state_min_seq(struct imm_state const* state) { return 0; }

static unsigned mute_state_max_seq(struct imm_state const* state) { return 0; }
