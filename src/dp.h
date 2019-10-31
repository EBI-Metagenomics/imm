#ifndef IMM_DP_H
#define IMM_DP_H

#include "hide.h"

struct imm_path;
struct imm_state;
struct mstate;

HIDE struct dp* imm_dp_create(struct mstate const* const* mm_states, int nstates,
                              char const* seq, struct imm_state const* end_state);
HIDE double     imm_dp_viterbi(struct dp* dp, struct imm_path* path);
HIDE void       imm_dp_destroy(struct dp* dp);

#endif
