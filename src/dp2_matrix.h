#ifndef IMM_DP2_MATRIX_H
#define IMM_DP2_MATRIX_H

struct dp2;
struct dp2_matrix;
struct imm_seq;
struct dp2_step;

struct dp2_matrix* dp2_matrix_new(struct dp2 const* dp);
void               dp2_matrix_setup(struct dp2_matrix* dp_matrix, struct imm_seq const* seq);
double             dp2_matrix_get_cost(struct dp2_matrix const* dp_matrix, unsigned row,
                                       struct dp2_step const* step);
void               dp2_matrix_set_cost(struct dp2_matrix const* dp_matrix, unsigned row,
                                       struct dp2_step const* step, double cost);
struct dp2_step*   dp2_matrix_get_prev_step(struct dp2_matrix const* dp_matrix, unsigned row,
                                            struct dp2_step const* step);
struct imm_seq const* dp2_matrix_get_seq(struct dp2_matrix const* dp_matrix);
void                  dp2_matrix_destroy(struct dp2_matrix const* matrix);

#endif
