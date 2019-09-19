#include "state/silent.h"

#include <math.h>

void silent_state_create(struct nhmm_state *state) { state->impl = NULL; }

double silent_state_emission_lprob(struct nhmm_state *state, const char *seq,
                                   size_t seq_len)
{
    if (seq_len == 0)
        return 0.0;
    return -INFINITY;
}

void silent_state_destroy(struct nhmm_state *state) {}
