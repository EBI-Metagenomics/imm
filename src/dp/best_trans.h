#ifndef DP_BEST_TRANS_H
#define DP_BEST_TRANS_H

#include "dp/assume.h"
#include "dp/final_score.h"
#include "dp/matrix.h"
#include "dp/minmax.h"
#include "dp/state_table.h"
#include "dp/trans_table.h"
#include "imm/dp.h"
#include "imm/dp/trans_table.h"

static inline void best_trans_score_loop(
    struct matrix const *matrix, struct imm_dp_trans_table const *trans_table,
    unsigned row, unsigned src, unsigned dst, struct span const *span,
    unsigned tidx, struct final_score *score)
{
    for (unsigned len = span->min; len <= span->max; ++len)
    {
        unsigned r = row - len;
        imm_float v0 = matrix_get_score(matrix, r, src, len);
        imm_float v1 = trans_table_score(trans_table, dst, tidx);
        imm_float v = v0 + v1;

        if (v > score->score)
        {
            score->score = v;
            score->state = src;
            score->emislen = len;
            score->trans = (uint16_t)tidx;
            score->seqlen = (uint8_t)(len - span->min);
        }
    }
}

static struct final_score best_trans_score(bool first_row, bool real_past,
                                           bool safe_floor, enum state_type T,
                                           unsigned MINLEN, unsigned MAXLEN,
                                           struct imm_dp const *dp,
                                           struct matrix const *matrix,
                                           unsigned dst, unsigned row)
{
    ASSUME_FIRST_ROW(first_row, row);
    struct final_score score = {imm_lprob_zero(), IMM_STATE_NULL_IDX,
                                IMM_STATE_NULL_SEQLEN, UINT16_MAX, UINT8_MAX};

    for (unsigned i = 0; i < trans_table_ntrans(&dp->trans_table, dst); ++i)
    {
        unsigned src = trans_table_source_state(&dp->trans_table, dst, i);
        ASSUME_REAL_PAST(real_past, src, dst);

        struct span span = state_table_span(&dp->state_table, src);

        ASSUME_MINLEN(T, MINLEN, span.min);
        ASSUME_SAFE_FLOOR(safe_floor, span.min, row);
        if (span.min > row || (span.min == 0 && src > dst)) continue;
        ASSUME_MIXLEN(T, MINLEN, MAXLEN, span.min, span.max);
        span.max = min_u(span.max, row);

        best_trans_score_loop(matrix, &dp->trans_table, row, src, dst, &span, i,
                              &score);
    }

    return score;
}

#endif