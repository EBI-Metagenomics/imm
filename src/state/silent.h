#ifndef IMM_STATE_SILENT_H
#define IMM_STATE_SILENT_H

#include "state/state.h"

void silent_state_create(struct imm_state *state);
double silent_state_emiss_lprob(const struct imm_state *state, const char *seq,
                                size_t seq_len);
int silent_state_normalize(struct imm_state *state);
int silent_state_min_seq(const struct imm_state *state);
int silent_state_max_seq(const struct imm_state *state);
void silent_state_destroy(struct imm_state *state);

#endif
