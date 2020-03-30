#ifndef IMM_DP_MATRIX_H
#define IMM_DP_MATRIX_H

#include "dp.h"
#include "dp_states.h"
#include "dp_step.h"
#include "eseq.h"
#include "matrixd.h"
#include "step_matrix.h"

struct dp;
struct imm_seq;

struct dp_matrix
{
    struct dp_states const* states;
    struct matrixd*         score;
    struct step_matrix*     prev_step;
    int*                    state_col;
    unsigned                nstates;
};

struct dp_matrix* dp_matrix_new(struct dp_states const* states);
void              dp_matrix_setup(struct dp_matrix* matrix, struct eseq const* eseq);

static inline double dp_matrix_get_score(struct dp_matrix const* dp_matrix, unsigned row,
                                         struct dp_step step)
{
    unsigned col = (unsigned)(dp_matrix->state_col[step.state] + (int)step.seq_len);
    return matrixd_get(dp_matrix->score, row, col);
}

static inline void dp_matrix_set_score(struct dp_matrix const* dp_matrix, unsigned row,
                                       struct dp_step step, double score)
{
    unsigned col = (unsigned)(dp_matrix->state_col[step.state] + (int)step.seq_len);
    matrixd_set(dp_matrix->score, row, col, score);
}

static inline struct dp_step* dp_matrix_get_prev_step(struct dp_matrix const* dp_matrix,
                                                      unsigned row, unsigned state)
{
    return step_matrix_get_ptr(dp_matrix->prev_step, row, state);
}

void dp_matrix_destroy(struct dp_matrix const* matrix);

#endif
