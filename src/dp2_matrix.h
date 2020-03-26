#ifndef IMM_DP2_MATRIX_H
#define IMM_DP2_MATRIX_H

#include "dp2.h"
#include "dp2_states.h"
#include "dp2_step.h"
#include "matrixd.h"
#include "step_matrix.h"

struct dp2;
struct imm_seq;
struct dp2_step;

struct dp2_matrix
{
    struct dp2_states const* states;
    struct imm_seq const*    seq;
    struct matrixd*          cost;
    struct step_matrix*      prev_step;
    unsigned*                state_col;
    unsigned                 nstates;
};

struct dp2_matrix* dp2_matrix_new(struct dp2_states const* states);
void               dp2_matrix_setup(struct dp2_matrix* dp_matrix, struct imm_seq const* seq);

static inline double dp2_matrix_get_cost(struct dp2_matrix const* dp_matrix, unsigned row,
                                         struct dp2_step const* step)
{
    unsigned min_seq = dp2_states_min_seq(dp_matrix->states, step->state);
    unsigned col = dp_matrix->state_col[step->state] + step->seq_len - min_seq;
    return *matrixd_get_c(dp_matrix->cost, row, col);
}

static inline void dp2_matrix_set_cost(struct dp2_matrix const* dp_matrix, unsigned row,
                                       struct dp2_step const* step, double cost)
{
    unsigned min_seq = dp2_states_min_seq(dp_matrix->states, step->state);
    unsigned col = dp_matrix->state_col[step->state] + step->seq_len - min_seq;
    *matrixd_get(dp_matrix->cost, row, col) = cost;
}

static inline struct dp2_step* dp2_matrix_get_prev_step(struct dp2_matrix const* dp_matrix,
                                                        unsigned row, unsigned state)
{
    return step_matrix_get(dp_matrix->prev_step, row, state);
}

static inline struct imm_seq const* dp2_matrix_get_seq(struct dp2_matrix const* dp_matrix)
{
    return dp_matrix->seq;
}

void dp2_matrix_destroy(struct dp2_matrix const* matrix);

#endif
