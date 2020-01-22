#ifndef IMM_DP_H
#define IMM_DP_H

struct imm_path;
struct imm_state;
struct mstate;

struct dp* imm_dp_create(struct mstate const* const* mm_states, int nstates, char const* seq,
                         struct imm_state const* end_state);
double     imm_dp_viterbi(struct dp* dp, struct imm_path* path);
void       imm_dp_destroy(struct dp* dp);

#endif
