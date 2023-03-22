#ifndef DP_MATRIX_H
#define DP_MATRIX_H

#include "imm/float.h"
#include "matrix/matrix.h"
#include <stdio.h>

struct imm_dp_state_table;

MAKE_MATRIX(matrixf, imm_float)

struct matrix
{
    struct matrixf score;
    int *state_col;
};

struct cell
{
    unsigned row;
    unsigned state;
    unsigned len;
};

static inline struct cell cell_init(unsigned row, unsigned state, unsigned len)
{
    return (struct cell){.row = row, .state = state, .len = len};
}

void imm_matrix_del(struct matrix const *matrix);

enum imm_rc imm_matrix_init(struct matrix *matrix,
                            struct imm_dp_state_table const *tbl);

enum imm_rc imm_matrix_reset(struct matrix *matrix,
                             struct imm_dp_state_table const *tbl);

static inline imm_float matrix_get_score(struct matrix const *x, struct cell y)
{
    y.row = y.row % matrixf_nrows(&x->score);
    int col = x->state_col[y.state] + (int)y.len;
    return matrixf_get(&x->score, y.row, (unsigned)col);
}

static inline void matrix_set_score(struct matrix *x, struct cell y,
                                    imm_float score)
{
    y.row = y.row % matrixf_nrows(&x->score);
    int col = x->state_col[y.state] + (int)y.len;
    matrixf_set(&x->score, y.row, (unsigned)col, score);
}

void imm_matrix_dump(struct matrix *matrix, FILE *restrict);

#endif
