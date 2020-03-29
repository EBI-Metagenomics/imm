#ifndef IMM_DP2_MATRIX_H
#define IMM_DP2_MATRIX_H

#include "dp2.h"
#include "dp2_states.h"
#include "dp2_step.h"
#include "eseq.h"
#include "matrixd.h"
#include "step_matrix.h"

struct dp2;
struct imm_seq;
struct dp2_step;

struct dp2_matrix
{
    struct dp2_states const* states;
    struct matrixd*          cost;
    struct step_matrix*      prev_step;
    int*                     state_col;
    unsigned                 nstates;
};

struct dp2_matrix* dp2_matrix_new(struct dp2_states const* states);
void               dp2_matrix_setup(struct dp2_matrix* matrix, struct eseq const* eseq);

static inline double dp2_matrix_get_cost(struct dp2_matrix const* dp_matrix, unsigned row,
                                         struct dp2_step step)
{
    unsigned col = (unsigned)(dp_matrix->state_col[step.state] + (int)step.seq_len);
    return matrixd_get(dp_matrix->cost, row, col);
}

static inline void dp2_matrix_set_cost(struct dp2_matrix const* dp_matrix, unsigned row,
                                       struct dp2_step step, double cost)
{
    unsigned col = (unsigned)(dp_matrix->state_col[step.state] + (int)step.seq_len);
    matrixd_set(dp_matrix->cost, row, col, cost);
}

static inline struct dp2_step* dp2_matrix_get_prev_step(struct dp2_matrix const* dp_matrix,
                                                        unsigned row, unsigned state)
{
    return step_matrix_get_ptr(dp_matrix->prev_step, row, state);
}

void dp2_matrix_destroy(struct dp2_matrix const* matrix);

#endif
