#ifndef NHMM_STATE_NORMAL_H
#define NHMM_STATE_NORMAL_H

#include "emission.h"
#include "state/state.h"

struct normal_state
{
    struct emission *emission;
};

void normal_state_create(struct nhmm_state *state, double *emission_lprobs);
double normal_state_emission_lprob(struct nhmm_state *state, const char* seq, size_t seq_len);
void normal_state_destroy(struct nhmm_state *state);

#endif
