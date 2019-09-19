#ifndef _NHMM_STATE_SILENT_H
#define _NHMM_STATE_SILENT_H

#include "emission.h"
#include "state/state.h"

void silent_state_create(struct nhmm_state *state);
double silent_state_emission_lprob(struct nhmm_state *state, const char *seq,
                                   size_t seq_len);
void silent_state_destroy(struct nhmm_state *state);

#endif
