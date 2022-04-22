#ifndef DP_MATRIX_H
#define DP_MATRIX_H

#include "imm/float.h"
#include "matrix/matrix.h"

struct imm_dp_state_table;

MAKE_MATRIX(matrixf, imm_float)

struct matrix
{
    struct matrixf score;
    unsigned *state_col;
};

void matrix_del(struct matrix const *matrix);

enum imm_rc matrix_init(struct matrix *matrix,
                        struct imm_dp_state_table const *tbl);

enum imm_rc matrix_reset(struct matrix *matrix,
                         struct imm_dp_state_table const *tbl);

static inline imm_float matrix_get_score(struct matrix const *matrix,
                                         unsigned row, unsigned state,
                                         unsigned seqlen)
{
    row = row % matrixf_nrows(&matrix->score);
    unsigned col = matrix->state_col[state] + seqlen;
    return matrixf_get(&matrix->score, row, col);
}

static inline void matrix_set_score(struct matrix *matrix, unsigned row,
                                    unsigned state, unsigned seqlen,
                                    imm_float score)
{
    row = row % matrixf_nrows(&matrix->score);
    unsigned col = matrix->state_col[state] + seqlen;
    matrixf_set(&matrix->score, row, col, score);
}

#endif
