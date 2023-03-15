#ifndef DP_BEST_TRANS_H
#define DP_BEST_TRANS_H

#include "dp/assume.h"
#include "dp/matrix.h"
#include "dp/minmax.h"
#include "dp/premise.h"
#include "dp/state_table.h"
#include "dp/trans_info.h"
#include "dp/trans_table.h"
#include "imm/dp.h"
#include "imm/dp/trans_table.h"

static struct trans_info best_trans_score_safe_past(struct imm_dp const *dp,
                                                    struct matrix const *matrix,
                                                    unsigned dst, unsigned row)
{
    assume(row > 0);

    struct imm_dp_state_table const *st = &dp->state_table;
    struct imm_dp_trans_table const *tt = &dp->trans_table;
    struct trans_info score = trans_info();

    for (unsigned i = 0; i < trans_table_ntrans(tt, dst); ++i)
    {
        unsigned src = trans_table_source_state(tt, dst, i);
        // if (premise.safe_odd_state) assume(src <= dst);

        struct span span = state_table_span(st, src);
        assume(row >= span.max);

        if (span.min == 0 && src > dst) continue;

        for (unsigned len = span.min; len <= span.max; ++len)
        {
            assert(row >= len);
            unsigned r = row - len;
            imm_float v0 = matrix_get_score(matrix, r, src, len);
            imm_float v1 = trans_table_score(tt, dst, i);
            imm_float v = v0 + v1;

            if (v > score.score)
            {
                score.score = v;
                score.prev_state = src;
                score.prev_seqlen = len;
                score.trans = (uint_fast16_t)i;
                assert(len >= span.min);
                score.seqlen = (uint_fast8_t)(len - span.min);
            }
        }
    }

    return score;
}

static struct trans_info best_trans_score3(struct imm_dp const *dp,
                                           struct matrix const *matrix,
                                           unsigned dst, unsigned row)
{
    struct imm_dp_state_table const *st = &dp->state_table;
    struct imm_dp_trans_table const *tt = &dp->trans_table;
    struct trans_info score = trans_info();
    if (row == 0 && st->start.state == dst)
    {
        score.score = st->start.lprob;
        return score;
    }

    for (unsigned i = 0; i < trans_table_ntrans(tt, dst); ++i)
    {
        unsigned src = trans_table_source_state(tt, dst, i);
        // if (premise.safe_odd_state) assume(src <= dst);

        struct span span = state_table_span(st, src);

        if (span.min > row) continue;
        if (span.min == 0 && src > dst) continue;
        if (span.max > row) span.max = row;

        for (unsigned len = span.min; len <= span.max; ++len)
        {
            assert(row >= len);
            unsigned r = row - len;
            imm_float v0 = matrix_get_score(matrix, r, src, len);
            imm_float v1 = trans_table_score(tt, dst, i);
            imm_float v = v0 + v1;

            if (v > score.score)
            {
                score.score = v;
                score.prev_state = src;
                score.prev_seqlen = len;
                score.trans = (uint_fast16_t)i;
                assert(len >= span.min);
                score.seqlen = (uint_fast8_t)(len - span.min);
            }
        }
    }

    return score;
}

#endif
