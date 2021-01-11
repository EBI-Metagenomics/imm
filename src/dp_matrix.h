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
    struct matrixu16*            prev_state;
    struct matrixu8*             prev_seq_len;
    int16_t*                     state_col;
    uint16_t                     nstates;
};

#define INVALID_STATE UINT16_MAX
#define INVALID_SEQ_LEN UINT8_MAX

struct dp_matrix*   dp_matrix_create(struct dp_state_table const* states);
void                dp_matrix_destroy(struct dp_matrix const* matrix);
static inline void  dp_matrix_get_prev_step(struct dp_matrix const* dp_matrix, uint_fast16_t row,
                                            uint_fast16_t state, uint_fast16_t* step_state,
                                            uint_fast8_t* step_seq_len);
static inline void  dp_matrix_set_prev_step(struct dp_matrix const* dp_matrix, uint_fast16_t row,
                                            uint_fast16_t state, uint_fast16_t step_state,
                                            uint_fast8_t step_seq_len);
static inline float dp_matrix_get_score(struct dp_matrix const* dp_matrix, uint_fast16_t row,
                                        uint_fast16_t state, uint_fast8_t seq_len);
static inline void  dp_matrix_set_score(struct dp_matrix const* dp_matrix, uint_fast16_t row,
                                        uint_fast16_t state, uint_fast8_t seq_len, float score);
void                dp_matrix_setup(struct dp_matrix* matrix, struct eseq const* eseq);

static inline void dp_matrix_get_prev_step(struct dp_matrix const* dp_matrix, uint_fast16_t row,
                                           uint_fast16_t state, uint_fast16_t* step_state,
                                           uint_fast8_t* step_seq_len)
{
    *step_state = matrixu16_get(dp_matrix->prev_state, row, state);
    *step_seq_len = matrixu8_get(dp_matrix->prev_seq_len, row, state);
}

static inline void dp_matrix_set_prev_step(struct dp_matrix const* dp_matrix, uint_fast16_t row,
                                           uint_fast16_t state, uint_fast16_t step_state,
                                           uint_fast8_t step_seq_len)
{
    matrixu16_set(dp_matrix->prev_state, row, state, (uint16_t)step_state);
    matrixu8_set(dp_matrix->prev_seq_len, row, state, (uint8_t)step_seq_len);
}

static inline float dp_matrix_get_score(struct dp_matrix const* dp_matrix, uint_fast16_t row,
                                        uint_fast16_t state, uint_fast8_t seq_len)
{
    uint_fast16_t col = (uint_fast16_t)(dp_matrix->state_col[state] + (int_fast16_t)seq_len);
    return matrixf_get(dp_matrix->score, row, col);
}

static inline void dp_matrix_set_score(struct dp_matrix const* dp_matrix, uint_fast16_t row,
                                       uint_fast16_t state, uint_fast8_t seq_len, float score)
{
    uint_fast16_t col = (uint_fast16_t)(dp_matrix->state_col[state] + (int_fast16_t)seq_len);
    matrixf_set(dp_matrix->score, row, col, score);
}

#endif
