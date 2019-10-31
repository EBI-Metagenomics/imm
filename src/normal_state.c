#include "imm/imm.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

struct imm_normal_state
{
    struct imm_state* interface;
    double*           lprobs;
};

static double normal_state_lprob(struct imm_state const* state, char const* seq, int seq_len);
static int    normal_state_min_seq(struct imm_state const* state);
static int    normal_state_max_seq(struct imm_state const* state);

struct imm_normal_state* imm_normal_state_create(char const* name, struct imm_abc const* abc,
                                                 double const* lprobs)
{
    struct imm_normal_state* state = malloc(sizeof(struct imm_normal_state));

    size_t len = (size_t)imm_abc_length(abc);
    state->lprobs = malloc(sizeof(double) * len);
    memcpy(state->lprobs, lprobs, sizeof(double) * len);

    struct imm_state_funcs funcs = {normal_state_lprob, normal_state_min_seq,
                                    normal_state_max_seq};
    state->interface = imm_state_create(name, abc, funcs, state);
    return state;
}

void imm_normal_state_destroy(struct imm_normal_state* state)
{
    if (!state) {
        imm_error("state should not be NULL");
        return;
    }

    imm_state_destroy(state->interface);
    state->interface = NULL;

    free(state->lprobs);
    state->lprobs = NULL;

    free(state);
}

int imm_normal_state_normalize(struct imm_normal_state* state)
{
    int len = imm_abc_length(imm_state_get_abc(imm_state_cast_c(state)));
    return imm_lprob_normalize(state->lprobs, len);
}

static double normal_state_lprob(struct imm_state const* state, char const* seq, int seq_len)
{
    struct imm_normal_state const* s = imm_state_get_impl_c(state);
    if (seq_len == 1) {
        struct imm_abc const* abc = imm_state_get_abc(state);
        if (imm_abc_has_symbol(abc, seq[0]))
            return s->lprobs[imm_abc_symbol_idx(abc, seq[0])];
    }
    return imm_lprob_zero();
}

static int normal_state_min_seq(struct imm_state const* state) { return 1; }

static int normal_state_max_seq(struct imm_state const* state) { return 1; }
