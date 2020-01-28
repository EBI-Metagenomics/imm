#ifndef IMM_DP_H
#define IMM_DP_H

struct imm_path;
struct imm_state;
struct imm_seq;
struct mstate;

struct dp* dp_create(struct mstate const* const* mm_states, unsigned nstates,
                     struct imm_seq const* seq, struct imm_state const* end_state);
double     dp_viterbi(struct dp* dp, struct imm_path* path);
void       dp_destroy(struct dp const* dp);

#endif
