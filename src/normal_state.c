#include "imm/normal_state.h"
#include "free.h"
#include "imm/abc.h"
#include "imm/lprob.h"
#include "imm/state.h"
#include <stdlib.h>
#include <string.h>

struct imm_normal_state
{
    struct imm_state const* interface;
    double*                 lprobs;
};

static double   normal_state_lprob(struct imm_state const* state, struct imm_seq const* seq);
static unsigned normal_state_min_seq(struct imm_state const* state);
static unsigned normal_state_max_seq(struct imm_state const* state);

struct imm_normal_state const* imm_normal_state_create(char const*           name,
                                                       struct imm_abc const* abc,
                                                       double const*         lprobs)
{
    struct imm_normal_state* state = malloc(sizeof(struct imm_normal_state));

    size_t len = imm_abc_length(abc);
    state->lprobs = malloc(sizeof(double) * len);
    memcpy(state->lprobs, lprobs, sizeof(double) * len);

    struct imm_state_funcs funcs = {normal_state_lprob, normal_state_min_seq,
                                    normal_state_max_seq};
    state->interface = imm_state_create(name, abc, funcs, state);
    return state;
}

void imm_normal_state_destroy(struct imm_normal_state const* state)
{
    imm_state_destroy(state->interface);
    imm_free(state->lprobs);
    imm_free(state);
}

static double normal_state_lprob(struct imm_state const* state, struct imm_seq const* seq)
{
    struct imm_normal_state const* s = imm_state_get_impl_c(state);
    if (imm_seq_length(seq) == 1) {
        struct imm_abc const* abc = imm_state_get_abc(state);
        int const             idx = imm_abc_symbol_idx(abc, imm_seq_string(seq)[0]);
        if (idx >= 0)
            return s->lprobs[idx];
    }
    return imm_lprob_zero();
}

static unsigned normal_state_min_seq(struct imm_state const* state) { return 1; }

static unsigned normal_state_max_seq(struct imm_state const* state) { return 1; }
