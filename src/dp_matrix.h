#ifndef IMM_DP_MATRIX_H
#define IMM_DP_MATRIX_H

#include "array.h"
#include "bug.h"
#include "free.h"
#include "imm/seq.h"
#include "matrix.h"

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

struct step
{
    struct state_info const* state;
    unsigned                 seq_len;
};

struct cell
{
    double      score;
    struct step prev_step;
};
MAKE_MATRIX_STRUCT(cell, struct cell)
MAKE_MATRIX_CREATE(cell, struct cell)
MAKE_MATRIX_GET(cell, struct cell)
MAKE_MATRIX_GET_C(cell, struct cell)
MAKE_MATRIX_DESTROY(cell)
MAKE_MATRIX_NROWS(cell)
MAKE_MATRIX_NCOLS(cell)
MAKE_MATRIX_RESIZE(cell, struct cell)

struct dp_matrix
{
    struct imm_seq const* seq;
    struct matrix_cell*   cell;
    unsigned*             state_col;
    unsigned              nstates;
};

static inline unsigned column(struct dp_matrix const* dp_matrix, struct step const* step)
{
    BUG(step->seq_len < step->state->min_seq);
    return dp_matrix->state_col[step->state->idx] + step->seq_len - step->state->min_seq;
}

static inline double get_score(struct dp_matrix const* dp_matrix, unsigned row,
                               struct step const* step)
{
    return matrix_cell_get_c(dp_matrix->cell, row, column(dp_matrix, step))->score;
}

struct dp_matrix* dp_matrix_create(struct dp const* dp, struct imm_seq const* seq);

void dp_matrix_reset(struct dp_matrix* dp_matrix, struct imm_seq const* seq);

void dp_matrix_destroy(struct dp_matrix const* matrix);

#endif
