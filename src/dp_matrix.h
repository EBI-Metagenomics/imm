#ifndef DP_MATRIX_H
#define DP_MATRIX_H

#include "dp.h"
#include "dp_state_table.h"
#include "eseq.h"
#include "matrix.h"
#include <inttypes.h>

struct imm_dp;
struct imm_seq;

struct dp_matrix
{
    struct dp_state_table const* states;
    struct matrixf*              score;
    int16_t*                     state_col;
    uint16_t                     nstates;
};

#define INVALID_STATE UINT16_MAX
#define INVALID_SEQ_LEN UINT8_MAX

struct dp_matrix*       dp_matrix_create(struct dp_state_table const* states);
void                    dp_matrix_destroy(struct dp_matrix const* matrix);
static inline imm_float dp_matrix_get_score(struct dp_matrix const* dp_matrix, uint_fast32_t row, unsigned state,
                                            unsigned seq_len);
static inline void      dp_matrix_set_score(struct dp_matrix const* dp_matrix, uint_fast32_t row, unsigned state,
                                            unsigned seq_len, imm_float score);

static inline imm_float dp_matrix_get_score(struct dp_matrix const* dp_matrix, uint_fast32_t row, unsigned state,
                                            unsigned seq_len)
{
    uint_fast32_t col = (uint_fast32_t)(dp_matrix->state_col[state] + (int_fast32_t)seq_len);
    return matrixf_get(dp_matrix->score, row % matrixf_nrows(dp_matrix->score), col);
}

static inline void dp_matrix_set_score(struct dp_matrix const* dp_matrix, uint_fast32_t row, unsigned state,
                                       unsigned seq_len, imm_float score)
{
    uint_fast32_t col = (uint_fast32_t)(dp_matrix->state_col[state] + (int_fast32_t)seq_len);
    matrixf_set(dp_matrix->score, row % matrixf_nrows(dp_matrix->score), col, score);
}

#endif
