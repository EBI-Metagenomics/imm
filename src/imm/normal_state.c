#include "imm.h"
#include "src/imm/abc.h"
#include "src/imm/hide.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

struct imm_normal_state
{
    double *lprobs;
    struct imm_state *interface;
};

HIDE double normal_state_lprob(const struct imm_state *state, const char *seq, int seq_len);
HIDE int normal_state_min_seq(const struct imm_state *state);
HIDE int normal_state_max_seq(const struct imm_state *state);

struct imm_normal_state *imm_normal_state_create(const char *name, const struct imm_abc *abc,
                                                 const double *lprobs)
{
    struct imm_normal_state *state = malloc(sizeof(struct imm_normal_state));

    size_t length = (size_t)abc_length(abc);
    state->lprobs = malloc(sizeof(double) * length);
    memcpy(state->lprobs, lprobs, sizeof(double) * length);

    struct imm_state_funcs funcs = {normal_state_lprob, normal_state_min_seq,
                                    normal_state_max_seq};
    state->interface = imm_state_create(name, abc, funcs, state);
    return state;
}

struct imm_state *imm_normal_state_cast(struct imm_normal_state *state)
{
    return state->interface;
}

const struct imm_state *imm_normal_state_cast_c(const struct imm_normal_state *state)
{
    return state->interface;
}

void imm_normal_state_destroy(struct imm_normal_state *state)
{
    if (!state)
        return;

    imm_state_destroy(state->interface);
    state->interface = NULL;

    free(state->lprobs);
    state->lprobs = NULL;
}

int imm_normal_state_normalize(struct imm_normal_state *state)
{

    int len = abc_length(imm_state_get_abc(state->interface));
    return imm_log_normalize(state->lprobs, len);
}

double normal_state_lprob(const struct imm_state *state, const char *seq, int seq_len)
{
    const struct imm_normal_state *s = imm_state_get_impl_c(state);
    if (seq_len == 1) {
        const struct imm_abc *abc = imm_state_get_abc(state);
        if (abc_has_symbol(abc, seq[0]))
            return s->lprobs[abc_symbol_idx(abc, seq[0])];
    }
    return -INFINITY;
}

int normal_state_min_seq(const struct imm_state *state) { return 1; }

int normal_state_max_seq(const struct imm_state *state) { return 1; }
