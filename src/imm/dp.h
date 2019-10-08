#ifndef IMM_DP_H
#define IMM_DP_H

#include "src/imm/hide.h"

struct mm_state;

HIDE struct dp *dp_create(const struct mm_state *mm_states, const char *seq, int seq_len);
HIDE double dp_viterbi(struct dp *dp);
HIDE void dp_destroy(struct dp *dp);

#endif