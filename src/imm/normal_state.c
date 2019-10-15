#include "imm.h"
#include "src/imm/abc.h"
#include "src/imm/hide.h"
#include "src/imm/state.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

struct imm_normal_state
{
    struct imm_state *interface;
    double *lprobs;
};

HIDE double normal_state_lprob(const struct imm_state *state, const char *seq, int seq_len);
HIDE int normal_state_min_seq(const struct imm_state *state);
HIDE int normal_state_max_seq(const struct imm_state *state);

struct imm_normal_state *imm_normal_state_create(const char *name, const struct imm_abc *abc,
                                                 const double *lprobs)
{
    struct imm_normal_state *state = malloc(sizeof(struct imm_normal_state));

    size_t len = (size_t)abc_length(abc);
    state->lprobs = malloc(sizeof(double) * len);
    memcpy(state->lprobs, lprobs, sizeof(double) * len);

    struct imm_state_funcs funcs = {normal_state_lprob, normal_state_min_seq,
                                    normal_state_max_seq};
    state->interface = imm_state_create(name, abc, funcs, state);
    return state;
}

void imm_normal_state_destroy(struct imm_normal_state *state)
{
    if (!state)
        return;

    imm_state_destroy(state->interface);
    state->interface = NULL;

    free(state->lprobs);
    state->lprobs = NULL;

    free(state);
}

int imm_normal_state_normalize(struct imm_normal_state *state)
{
    int len = abc_length(imm_state_get_abc(imm_state_cast_c(state)));
    return imm_lognormalize(state->lprobs, len);
}

double normal_state_lprob(const struct imm_state *state, const char *seq, int seq_len)
{
    const struct imm_normal_state *s = imm_state_get_impl_c(state);
    if (seq_len == 1) {
        const struct imm_abc *abc = imm_state_get_abc(state);
        if (abc_has_symbol(abc, seq[0]))
            return s->lprobs[abc_symbol_idx(abc, seq[0])];
    }
    return LOG0;
}

int normal_state_min_seq(const struct imm_state *state) { return 1; }

int normal_state_max_seq(const struct imm_state *state) { return 1; }
