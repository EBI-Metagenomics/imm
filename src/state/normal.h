#ifndef IMM_STATE_NORMAL_H
#define IMM_STATE_NORMAL_H

#include "state/state.h"

struct normal_state;

void normal_state_create(struct imm_state *state, const double *emiss_lprobs);
double normal_state_emiss_lprob(const struct imm_state *state, const char *seq,
                                int seq_len);
int normal_state_normalize(struct imm_state *state);
int normal_state_min_seq(const struct imm_state *state);
int normal_state_max_seq(const struct imm_state *state);
void normal_state_destroy(struct imm_state *state);

#endif
