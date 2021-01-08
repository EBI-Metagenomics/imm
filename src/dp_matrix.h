#ifndef DP_MATRIX_H
#define DP_MATRIX_H

#include "dp.h"
#include "dp_state_table.h"
#include "dp_step.h"
#include "eseq.h"
#include "matrix.h"
#include "step_matrix.h"
#include <inttypes.h>

struct imm_dp;
struct imm_seq;

struct dp_matrix
{
    struct dp_state_table const* states;
    struct matrixf*              score;
    struct step_matrix*          prev_step;
    int16_t*                     state_col;
    uint16_t                     nstates;
};

struct dp_matrix*                   dp_matrix_create(struct dp_state_table const* states);
void                                dp_matrix_destroy(struct dp_matrix const* matrix);
static inline struct dp_step*       dp_matrix_get_prev_step(struct dp_matrix const* dp_matrix,
                                                            unsigned row, uint16_t state);
static inline struct dp_step const* dp_matrix_get_prev_step_c(struct dp_matrix const* dp_matrix,
                                                              unsigned row, uint16_t state);
static inline float dp_matrix_get_score(struct dp_matrix const* dp_matrix, unsigned row,
                                        struct dp_step step);
static inline void  dp_matrix_set_score(struct dp_matrix const* dp_matrix, unsigned row,
                                        struct dp_step step, float score);
void                dp_matrix_setup(struct dp_matrix* matrix, struct eseq const* eseq);

static inline struct dp_step* dp_matrix_get_prev_step(struct dp_matrix const* dp_matrix,
                                                      unsigned row, uint16_t state)
{
    return step_matrix_get_ptr(dp_matrix->prev_step, row, state);
}

static inline struct dp_step const* dp_matrix_get_prev_step_c(struct dp_matrix const* dp_matrix,
                                                              unsigned row, uint16_t state)
{
    return step_matrix_get_ptr_c(dp_matrix->prev_step, row, state);
}

static inline float dp_matrix_get_score(struct dp_matrix const* dp_matrix, unsigned row,
                                        struct dp_step step)
{
    unsigned col = (unsigned)(dp_matrix->state_col[step.state] + (int)step.seq_len);
    return matrixf_get(dp_matrix->score, row, col);
}

static inline void dp_matrix_set_score(struct dp_matrix const* dp_matrix, unsigned row,
                                       struct dp_step step, float score)
{
    unsigned col = (unsigned)(dp_matrix->state_col[step.state] + (int)step.seq_len);
    matrixf_set(dp_matrix->score, row, col, score);
}

#endif
