#ifndef NHMM_STATE_NORMAL_H
#define NHMM_STATE_NORMAL_H

#include "emission.h"
#include "state/state.h"

struct normal_state;

void normal_state_create(struct nhmm_state *state, double *emiss_lprobs);
double normal_state_emiss_lprob(const struct nhmm_state *state, const char *seq,
                                size_t seq_len);
int normal_state_normalize(struct nhmm_state *state);
void normal_state_destroy(struct nhmm_state *state);

#endif
