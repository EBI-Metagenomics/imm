#ifndef NHMM_STATE_SILENT_H
#define NHMM_STATE_SILENT_H

#include "emission.h"
#include "state/state.h"

void silent_state_create(struct nhmm_state *state);
double silent_state_emiss_lprob(const struct nhmm_state *state, const char *seq,
                                size_t seq_len);
int silent_state_normalize(struct nhmm_state *state);
void silent_state_destroy(struct nhmm_state *state);

#endif
