#ifndef IMM_DP_MATRIX_H
#define IMM_DP_MATRIX_H

#include "array.h"
#include "free.h"
#include "imm/bug.h"
#include "imm/seq.h"
#include "make_matrix.h"

struct dp;

struct trans
{
    struct state_info const* src_state;
    double                   lprob;
};
MAKE_ARRAY_STRUCT(trans, struct trans)
MAKE_ARRAY_INIT(trans)
MAKE_ARRAY_APPEND(trans, struct trans)
MAKE_ARRAY_GET_C(trans, struct trans)
MAKE_ARRAY_LENGTH(trans)
MAKE_ARRAY_EMPTY(trans)

struct state_info
{
    struct imm_state const* state;
    unsigned                min_seq;
    unsigned                max_seq;
    double                  start_lprob;
    unsigned                idx;
    struct array_trans      incoming_transitions;
};

struct dp_step
{
    struct state_info const* state;
    unsigned                 seq_len;
};

struct cell
{
    double      score;
    struct dp_step prev_step;
};
MAKE_MATRIX_STRUCT(matrix_cell, struct cell)
MAKE_MATRIX_CREATE(matrix_cell, struct cell)
MAKE_MATRIX_GET_PTR(matrix_cell, struct cell)
MAKE_MATRIX_GET_PTR_C(matrix_cell, struct cell)
MAKE_MATRIX_DESTROY(matrix_cell)
MAKE_MATRIX_NROWS(matrix_cell)
MAKE_MATRIX_NCOLS(matrix_cell)
MAKE_MATRIX_RESIZE(matrix_cell, struct cell)

struct dp_matrix
{
    struct imm_seq const* seq;
    struct matrix_cell*   cell;
    unsigned*             state_col;
    unsigned              nstates;
};

static inline unsigned column(struct dp_matrix const* dp_matrix, struct dp_step const* step)
{
    IMM_BUG(step->seq_len < step->state->min_seq);
    return dp_matrix->state_col[step->state->idx] + step->seq_len - step->state->min_seq;
}

static inline double get_score(struct dp_matrix const* dp_matrix, unsigned row,
                               struct dp_step const* step)
{
    return matrix_cell_get_ptr_c(dp_matrix->cell, row, column(dp_matrix, step))->score;
}

struct dp_matrix* dp_matrix_new(struct dp const* dp);

void dp_matrix_set(struct dp_matrix* dp_matrix, struct imm_seq const* seq);

void dp_matrix_destroy(struct dp_matrix const* matrix);

#endif
