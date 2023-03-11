#ifndef DP_BEST_TRANS_H
#define DP_BEST_TRANS_H

#include "dp/assume.h"
#include "dp/final_score.h"
#include "dp/matrix.h"
#include "dp/minmax.h"
#include "dp/premise.h"
#include "dp/state_table.h"
#include "dp/trans_table.h"
#include "imm/dp.h"
#include "imm/dp/trans_table.h"

static inline void best_trans_score_loop(
    struct matrix const *matrix, struct imm_dp_trans_table const *trans_table,
    unsigned row, unsigned src, unsigned dst, struct span const *span,
    unsigned tidx, struct final_score2 *score)
{
    for (unsigned len = span->min; len <= span->max; ++len)
    {
        assert(row >= len);
        unsigned r = row - len;
        imm_float v0 = matrix_get_score(matrix, r, src, len);
        imm_float v1 = trans_table_score(trans_table, dst, tidx);
        imm_float v = v0 + v1;

        if (v > score->score)
        {
            score->score = v;
            score->prev_state = src;
            score->prev_seqlen = len;
            score->trans = (uint16_t)tidx;
            assert(len >= span->min);
            score->seqlen = (uint8_t)(len - span->min);
        }
    }
}

static struct final_score2 best_trans_score3(struct premise premise,
                                             struct imm_dp const *dp,
                                             struct matrix const *matrix,
                                             unsigned dst, unsigned row)
{
    if (premise.first_row) ASSUME(row == 0);
    struct final_score2 score = {imm_lprob_zero(), IMM_STATE_NULL_IDX,
                                 IMM_STATE_NULL_SEQLEN, UINT16_MAX, UINT8_MAX};

    for (unsigned i = 0; i < trans_table_ntrans(&dp->trans_table, dst); ++i)
    {
        unsigned src = trans_table_source_state(&dp->trans_table, dst, i);
        if (premise.safe_past) ASSUME(src <= dst);

        struct span span = state_table_span(&dp->state_table, src);

        if (row == 0)
        {
            if (span.min > 0 || src > dst) continue;
            if (span.max > 0) printf("XXX: span.max: %u\n", span.max);
            best_trans_score_loop(matrix, &dp->trans_table, row, src, dst,
                                  &span, i, &score);
        }

#if 0
        ASSUME_MIXLEN(premise, span.min, span.max);
        if (premise.safe_floor) ASSUME(span.min <= row);

        if (span.min > row) continue;
        if (span.min == 0 && src > dst) continue;
        // This is used by `best_trans_score_first_row`
        // if (span.min > row || src > dst) continue;

        span.max = min_u(span.max, row);

        best_trans_score_loop(matrix, &dp->trans_table, row, src, dst, &span, i,
                              &score);
#endif
    }

    return score;
}

#endif
