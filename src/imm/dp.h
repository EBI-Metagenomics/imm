#ifndef IMM_DP_H
#define IMM_DP_H

#include "src/imm/hide.h"

struct imm_path;
struct imm_state;
struct mm_state;

HIDE struct dp *dp_create(const struct mm_state *const *mm_states, int nstates,
                          const char *seq, struct imm_state const *end_state);
HIDE double dp_viterbi(struct dp *dp, struct imm_path *path);
HIDE void dp_destroy(struct dp *dp);

#endif
