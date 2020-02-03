#ifndef IMM_DP_H
#define IMM_DP_H

struct dp;
struct dp_matrix;
struct imm_path;
struct imm_seq;
struct imm_state;
struct mstate;

struct dp*        dp_create(struct mstate const* const* mm_states, unsigned nstates,
                            struct imm_seq const* seq, struct imm_state const* end_state);
struct dp_matrix* dp_matrix_create(struct dp const* dp, struct imm_seq const* seq);
void              dp_matrix_destroy(struct dp_matrix const* matrix);
double dp_viterbi(struct dp const* dp, struct dp_matrix* matrix, struct imm_path* path);
void   dp_destroy(struct dp const* dp);

#endif
