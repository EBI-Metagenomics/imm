#include "dp2_matrix.h"
#include "dp2.h"
#include "dp2_step.h"
#include "imm/seq.h"
#include "matrixd.h"
#include "step_matrix.h"

struct dp2_matrix
{
    struct dp2 const*     dp;
    struct imm_seq const* seq;
    struct matrixd*       cost;
    struct step_matrix*   prev_step;
    unsigned*             state_col;
    unsigned              nstates;
};

struct dp2_matrix* dp2_matrix_new(struct dp2 const* dp)
{
    struct dp2_matrix* matrix = malloc(sizeof(struct dp2_matrix));

    matrix->dp = dp;
    matrix->seq = NULL;
    matrix->state_col = malloc(sizeof(unsigned) * dp2_nstates(dp));
    matrix->nstates = dp2_nstates(dp);

    unsigned next_col = 0;
    for (unsigned i = 0; i < matrix->nstates; ++i) {
        matrix->state_col[i] = next_col;
        next_col += dp2_state_max_seq(dp, i) - dp2_state_min_seq(dp, i) + 1;
    }

    matrix->cost = matrixd_create(1, next_col);
    matrix->prev_step = step_matrix_create(1, next_col);

    return matrix;
}

void dp2_matrix_setup(struct dp2_matrix* matrix, struct imm_seq const* seq)
{
    matrixd_resize(matrix->cost, imm_seq_length(seq) + 1, matrixd_ncols(matrix->cost));
    step_matrix_resize(matrix->prev_step, imm_seq_length(seq) + 1,
                       matrixd_ncols(matrix->cost));
    matrix->seq = seq;
}

double dp2_matrix_get_cost(struct dp2_matrix const* dp_matrix, unsigned row,
                           struct dp2_step const* step)
{
    unsigned min_seq = dp2_state_min_seq(dp_matrix->dp, step->state);
    unsigned col = dp_matrix->state_col[step->state] + step->seq_len - min_seq;
    return *matrixd_get_c(dp_matrix->cost, row, col);
}

void dp2_matrix_set_cost(struct dp2_matrix const* dp_matrix, unsigned row,
                         struct dp2_step const* step, double cost)
{
    unsigned min_seq = dp2_state_min_seq(dp_matrix->dp, step->state);
    unsigned col = dp_matrix->state_col[step->state] + step->seq_len - min_seq;
    *matrixd_get(dp_matrix->cost, row, col) = cost;
}

struct dp2_step* dp2_matrix_get_prev_step(struct dp2_matrix const* dp_matrix, unsigned row,
                                          struct dp2_step const* step)
{
    unsigned min_seq = dp2_state_min_seq(dp_matrix->dp, step->state);
    unsigned col = dp_matrix->state_col[step->state] + step->seq_len - min_seq;
    return step_matrix_get(dp_matrix->prev_step, row, col);
}

struct imm_seq const* dp2_matrix_get_seq(struct dp2_matrix const* dp_matrix)
{
    return dp_matrix->seq;
}

void dp2_matrix_destroy(struct dp2_matrix const* matrix)
{
    matrixd_destroy(matrix->cost);
    step_matrix_destroy(matrix->prev_step);
    imm_free(matrix->state_col);
    imm_free(matrix);
}
