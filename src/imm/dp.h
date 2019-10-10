#ifndef IMM_DP_H
#define IMM_DP_H

#include "src/imm/hide.h"

struct imm_state;
struct mm_state;

HIDE struct dp *dp_create(const struct mm_state *mm_states, const char *seq);
HIDE double dp_viterbi(struct dp *dp, const struct imm_state *end_state);
HIDE void dp_destroy(struct dp *dp);

#endif
