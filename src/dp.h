#ifndef IMM_DP_H
#define IMM_DP_H

struct mstate;
struct imm_state;
struct imm_abc;
struct dp_matrix;
struct dp_states;
struct seq_code;
struct eseq;
struct imm_path;

struct dp const* dp_create(struct imm_abc const* abc, struct mstate const* const* mstates,
                             unsigned const nstates, struct imm_state const* end_state);
struct dp_states const* dp_states(struct dp const* dp);
struct seq_code const*  dp_seq_code(struct dp const* dp);
double dp_viterbi(struct dp const* dp, struct dp_matrix* matrix, struct eseq const* eseq,
                   struct imm_path* path);
void   dp_destroy(struct dp const* dp);

#endif
