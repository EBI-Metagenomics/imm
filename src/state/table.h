#ifndef IMM_STATE_TABLE_H
#define IMM_STATE_TABLE_H

#include "state/state.h"

struct table_state;

void table_state_create(struct imm_state *state);
void table_state_add(struct table_state *state, const char *seq, int seq_len,
                     double lprob);
double table_state_emiss_lprob(const struct imm_state *state, const char *seq,
                               int seq_len);
int table_state_normalize(struct imm_state *state);
int table_state_min_seq(const struct imm_state *state);
int table_state_max_seq(const struct imm_state *state);
void table_state_destroy(struct imm_state *state);

#endif
