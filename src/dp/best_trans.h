#ifndef DP_BEST_TRANS_H
#define DP_BEST_TRANS_H

#include "dp/final_score.h"
#include "dp/matrix.h"
#include "dp/minmax.h"
#include "dp/state_table.h"
#include "dp/trans_table.h"
#include "imm/dp.h"
#include "task.h"

static struct final_score best_trans(struct imm_dp const *dp,
                                     struct matrix const *matrix, unsigned dst,
                                     unsigned row, uint16_t *best_trans,
                                     uint8_t *best_len)
{
    imm_float score = imm_lprob_zero();
    unsigned prev_state = IMM_STATE_NULL_IDX;
    unsigned prev_seqlen = IMM_STATE_NULL_SEQLEN;
    *best_trans = UINT16_MAX;
    *best_len = UINT8_MAX;

    for (unsigned i = 0; i < trans_table_ntrans(&dp->trans_table, dst); ++i)
    {
        unsigned src = trans_table_source_state(&dp->trans_table, dst, i);
        struct span span = state_table_span(&dp->state_table, src);

        if (imm_unlikely(row < span.min)) continue;

        span.max = (unsigned)MIN(span.max, row);
        for (unsigned len = span.min; len <= span.max; ++len)
        {
            imm_float v0 = matrix_get_score(matrix, row - len, src, len);
            imm_float v1 = trans_table_score(&dp->trans_table, dst, i);
            imm_float v = v0 + v1;

            if (v > score)
            {
                score = v;
                prev_state = src;
                prev_seqlen = len;
                *best_trans = (uint16_t)i;
                *best_len = (uint8_t)(len - span.min);
            }
        }
    }

    return (struct final_score){score, prev_state, prev_seqlen};
}

static struct final_score best_trans_row0(struct imm_dp const *dp,
                                          struct matrix const *matrix,
                                          unsigned dst, uint16_t *best_trans,
                                          uint8_t *best_len)
{
    imm_float score = imm_lprob_zero();
    unsigned prev_state = IMM_STATE_NULL_IDX;
    unsigned prev_seq_len = IMM_STATE_NULL_SEQLEN;
    *best_trans = UINT16_MAX;
    *best_len = UINT8_MAX;

    for (unsigned i = 0; i < trans_table_ntrans(&dp->trans_table, dst); ++i)
    {
        unsigned src = trans_table_source_state(&dp->trans_table, dst, i);
        unsigned min_seq = state_table_span(&dp->state_table, src).min;

        if (min_seq > 0 || src > dst) continue;

        imm_float v0 = matrix_get_score(matrix, 0, src, 0);
        imm_float v1 = trans_table_score(&dp->trans_table, dst, i);
        imm_float v = v0 + v1;

        if (v > score)
        {
            score = v;
            prev_state = src;
            prev_seq_len = 0;
            *best_trans = (uint16_t)i;
            *best_len = 0;
        }
    }

    return (struct final_score){score, prev_state, prev_seq_len};
}

#endif
