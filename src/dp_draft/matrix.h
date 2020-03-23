#ifndef IMM_DP_MATRIX_H
#define IMM_DP_MATRIX_H

#include "dp/step.h"
#include "matrix_double.h"

struct dp_matrix
{
    struct matrix_double*  cost;
    struct dp_step_matrix* step;
    unsigned const*        min_seq;
    unsigned const*        state_col;
    struct imm_seq const*  seq;
};

static inline double dp_matrix_cost(struct dp_matrix const* dp_matrix, unsigned row,
                                    struct dp_step step)
{
    unsigned col =
        dp_matrix->state_col[step.state] + step.seq_len - dp_matrix->min_seq[step.state];
    return dp_step_matrix_get_c(dp_matrix->cost, row, col);
}

#endif
