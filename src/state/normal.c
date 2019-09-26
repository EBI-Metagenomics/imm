#include "state/normal.h"
#include "alphabet.h"
#include "logaddexp.h"
#include "report.h"
#include <math.h>
#include <stdlib.h>

struct normal_state
{
    double *emiss_lprobs;
};

void normal_state_create(struct nhmm_state *state, double *emiss_lprobs)
{
    struct normal_state *s = malloc(sizeof(struct normal_state));

    size_t length = nhmm_alphabet_length(state->alphabet);
    s->emiss_lprobs = malloc(sizeof(double) * length);
    memcpy(s->emiss_lprobs, emiss_lprobs, sizeof(double) * length);

    state->impl = s;
}

double normal_state_emiss_lprob(const struct nhmm_state *state, const char *seq,
                                size_t seq_len)
{
    struct normal_state *s = state->impl;
    if (seq_len == 1) {
        if (alphabet_has_symbol(state->alphabet, seq[0]))
            return s->emiss_lprobs[alphabet_symbol_idx(state->alphabet, seq[0])];
    }
    return -INFINITY;
}

int normal_state_normalize(struct nhmm_state *state)
{

    size_t length = nhmm_alphabet_length(nhmm_state_get_alphabet(state));
    struct normal_state *s = state->impl;
    double lnorm = s->emiss_lprobs[0];
    for (size_t i = 1; i < length; ++i)
        lnorm = logaddexp(lnorm, s->emiss_lprobs[i]);

    if (!isfinite(lnorm)) {
        error("zero-probability alphabet");
        return -1;
    }

    for (size_t i = 1; i < length; ++i)
        s->emiss_lprobs[i] -= lnorm;
    
    return 0;
}

void normal_state_destroy(struct nhmm_state *state)
{
    if (!state)
        return;

    if (!state->impl)
        return;

    struct normal_state *s = state->impl;
    if (s->emiss_lprobs) {
        free(s->emiss_lprobs);
        s->emiss_lprobs = NULL;
    }

    free(state->impl);
    state->impl = NULL;
}
