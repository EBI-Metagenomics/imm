#ifndef DP_MATRIX_H
#define DP_MATRIX_H

#include "imm/float.h"
#include "matrix/matrix.h"

struct state_table;

MAKE_MATRIX(matrixf, imm_float)

struct matrix
{
    struct matrixf score;
    int16_t *state_col;
};

#define INVALID_STATE UINT16_MAX
#define INVALID_SEQLEN UINT8_MAX

void matrix_deinit(struct matrix const *matrix);

void matrix_init(struct matrix *matrix, struct state_table const *tbl);

static inline imm_float matrix_get_score(struct matrix const *matrix,
                                         unsigned row, unsigned state,
                                         unsigned seqlen)
{
    row = row % matrixf_nrows(&matrix->score);
    int col = matrix->state_col[state] + (int)seqlen;
    return matrixf_get(&matrix->score, row, (unsigned)col);
}

static inline void matrix_set_score(struct matrix *matrix, unsigned row,
                                    unsigned state, unsigned seqlen,
                                    imm_float score)
{
    row = row % matrixf_nrows(&matrix->score);
    int col = matrix->state_col[state] + (int)seqlen;
    matrixf_set(&matrix->score, row, (unsigned)col, score);
}

#endif
