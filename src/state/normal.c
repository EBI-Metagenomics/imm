#include "state/normal.h"
#include "alphabet.h"
#include <math.h>
#include <stdlib.h>

struct normal_state
{
    struct emission *emission;
};

void normal_state_create(struct nhmm_state *state, double *emiss_lprobs)
{
    struct normal_state *s = malloc(sizeof(struct normal_state));
    s->emission = emission_create(emiss_lprobs, nhmm_alphabet_length(state->alphabet));
    state->impl = s;
}

double normal_state_emiss_lprob(const struct nhmm_state *state, const char *seq,
                                size_t seq_len)
{
    struct normal_state *s = state->impl;
    if (seq_len == 1) {
        if (alphabet_has_symbol(state->alphabet, seq[0]))
            return s->emission->lprobs[alphabet_symbol_idx(state->alphabet, seq[0])];
    }
    return -INFINITY;
}

int normal_state_normalize(struct nhmm_state *state) {}

void normal_state_destroy(struct nhmm_state *state)
{
    if (!state)
        return;

    if (!state->impl)
        return;

    struct normal_state *s = state->impl;
    emission_destroy(s->emission);

    free(state->impl);
}
