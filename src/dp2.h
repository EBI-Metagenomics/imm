#ifndef IMM_DP2_H
#define IMM_DP2_H

struct mstate;
struct imm_state;
struct imm_abc;
struct dp2_matrix;
struct dp2_states;
struct seq_code;

struct dp2 const* dp2_create(struct imm_abc const* abc, struct mstate const* const* mstates,
                             unsigned const nstates, struct imm_state const* end_state);
struct seq_code const*   dp2_seq_code(struct dp2 const* dp);
struct dp2_states const* dp2_states(struct dp2 const* dp);
double                   dp2_viterbi(struct dp2 const* dp, struct dp2_matrix* matrix);
void                     dp2_destroy(struct dp2 const* dp);

#endif
