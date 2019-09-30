#ifndef NHMM_STATE_FRAME_H
#define NHMM_STATE_FRAME_H

#include "state/state.h"

struct frame_state;

void frame_state_create(struct nhmm_state *state, const double *base_emiss_lprobs,
                        const struct nhmm_codon *codon, double epsilon);
double frame_state_emiss_lprob(const struct nhmm_state *state, const char *seq,
                               size_t seq_len);
int frame_state_normalize(struct nhmm_state *state);
void frame_state_destroy(struct nhmm_state *state);

#endif
