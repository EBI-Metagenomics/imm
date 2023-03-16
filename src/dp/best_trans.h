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

static inline imm_float candidate_score(struct matrix const *matrix,
                                        struct imm_dp_trans_table const *tt,
                                        unsigned row, unsigned src,
                                        unsigned dst, unsigned len,
                                        unsigned tidx)
{
    unsigned r = row - len;
    imm_float v0 = matrix_get_score(matrix, r, src, len);
    imm_float v1 = trans_table_score(tt, dst, tidx);
    return v0 + v1;
}

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
            imm_float v = candidate_score(matrix, tt, row, src, dst, len, i);
            update_trans_info(&score, v, src, len, i, len - span.min);
        }
    }

    return score;
}

static struct trans_info best_trans_score(struct imm_dp const *dp,
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
            imm_float v = candidate_score(matrix, tt, row, src, dst, len, i);
            update_trans_info(&score, v, src, len, i, len - span.min);
        }
    }

    return score;
}

#endif
