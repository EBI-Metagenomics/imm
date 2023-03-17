#include "dp/best_trans.h"
#include "dp/final_score.h"
#include "dp/matrix.h"
#include "dp/minmax.h"
#include "dp/state_table.h"
#include "dp/trans_table.h"
#include "imm/dp.h"
#include "task.h"

static inline void best_trans_set(struct best_trans *x, imm_float score,
                                  unsigned prev_state, unsigned prev_len,
                                  unsigned trans, unsigned len)
{
    x->score = score;
    x->prev_state = prev_state;
    x->prev_len = prev_len;
    x->trans = trans;
    x->len = len;
}

static inline imm_float calc_score(struct matrix const *mt,
                                   struct imm_dp_trans_table const *tt,
                                   unsigned row, unsigned src, unsigned dst,
                                   unsigned len, unsigned trans)
{
    imm_float v0 = matrix_get_score(mt, row, src, len);
    imm_float v1 = trans_table_score(tt, dst, trans);
    return v0 + v1;
}

struct best_trans best_trans_find_ge1(struct imm_dp const *dp,
                                      struct matrix const *mt, unsigned dst,
                                      unsigned row)
{
    struct best_trans x = best_trans_init();
    struct imm_dp_trans_table const *tt = &dp->trans_table;

    for (unsigned i = 0; i < trans_table_ntrans(tt, dst); ++i)
    {
        unsigned src = trans_table_source_state(tt, dst, i);
        struct span span = state_table_span(&dp->state_table, src);
        if (span.min == 0) continue;

        if (imm_unlikely(row < span.min)) continue;

        span.max = MIN(span.max, row);
        for (unsigned len = span.min; len <= span.max; ++len)
        {
            assume(row >= len);
            assume(len >= span.min);
            imm_float v = calc_score(mt, tt, row - len, src, dst, len, i);
            if (v > x.score) best_trans_set(&x, v, src, len, i, len - span.min);
        }
    }

    return x;
}

struct best_trans best_trans_find(struct imm_dp const *dp,
                                  struct matrix const *mt, unsigned dst,
                                  unsigned row)
{
    struct best_trans x = best_trans_init();
    struct imm_dp_trans_table const *tt = &dp->trans_table;

    for (unsigned i = 0; i < trans_table_ntrans(tt, dst); ++i)
    {
        unsigned src = trans_table_source_state(tt, dst, i);
        struct span span = state_table_span(&dp->state_table, src);

        if (imm_unlikely(row < span.min)) continue;

        span.max = MIN(span.max, row);
        for (unsigned len = span.min; len <= span.max; ++len)
        {
            assume(row >= len);
            assume(len >= span.min);
            imm_float v = calc_score(mt, tt, row - len, src, dst, len, i);
            if (v > x.score) best_trans_set(&x, v, src, len, i, len - span.min);
        }
    }

    return x;
}

static inline void best_trans_find_safe_single_ge1(struct best_trans *x,
                                                   struct imm_dp const *dp,
                                                   struct matrix const *mt,
                                                   unsigned row, unsigned src,
                                                   unsigned dst, unsigned trans)
{
    struct imm_dp_trans_table const *tt = &dp->trans_table;
    struct span span = state_table_span(&dp->state_table, src);
    if (span.min == 0) return;

    assume(span.min >= 0);
    assume(row >= span.min);
    assume(row >= span.max);

    for (unsigned len = span.min; len <= span.max; ++len)
    {
        assume(row >= len);
        assume(len >= span.min);
        imm_float v = calc_score(mt, tt, row - len, src, dst, len, trans);
        if (v > x->score) best_trans_set(x, v, src, len, trans, len - span.min);
    }
}

static inline void best_trans_find_safe_single(struct best_trans *x,
                                               struct imm_dp const *dp,
                                               struct matrix const *mt,
                                               unsigned row, unsigned src,
                                               unsigned dst, unsigned trans)
{
    struct imm_dp_trans_table const *tt = &dp->trans_table;
    struct span span = state_table_span(&dp->state_table, src);

    assume(span.min >= 0);
    assume(row >= span.min);
    assume(row >= span.max);

    for (unsigned len = span.min; len <= span.max; ++len)
    {
        assume(row >= len);
        assume(len >= span.min);
        imm_float v = calc_score(mt, tt, row - len, src, dst, len, trans);
        if (v > x->score) best_trans_set(x, v, src, len, trans, len - span.min);
    }
}

struct best_trans best_trans_find_safe_ge1(struct imm_dp const *dp,
                                           struct matrix const *mt,
                                           unsigned dst, unsigned row)
{
    struct best_trans x = best_trans_init();
    struct imm_dp_trans_table const *tt = &dp->trans_table;

    for (unsigned i = 0; i < trans_table_ntrans(tt, dst); ++i)
    {
        unsigned src = trans_table_source_state(tt, dst, i);
        best_trans_find_safe_single_ge1(&x, dp, mt, row, src, dst, i);
    }

    return x;
}

struct best_trans best_trans_find_safe(struct imm_dp const *dp,
                                       struct matrix const *mt, unsigned dst,
                                       unsigned row)
{
    struct best_trans x = best_trans_init();
    struct imm_dp_trans_table const *tt = &dp->trans_table;

    for (unsigned i = 0; i < trans_table_ntrans(tt, dst); ++i)
    {
        unsigned src = trans_table_source_state(tt, dst, i);
        best_trans_find_safe_single(&x, dp, mt, row, src, dst, i);
    }

    return x;
}

struct best_trans best_trans_find_row0(struct imm_dp const *dp,
                                       struct matrix const *mt, unsigned dst)
{
    struct best_trans x = best_trans_init();
    struct imm_dp_trans_table const *tt = &dp->trans_table;

    for (unsigned i = 0; i < trans_table_ntrans(tt, dst); ++i)
    {
        unsigned src = trans_table_source_state(tt, dst, i);
        struct span span = state_table_span(&dp->state_table, src);

        if (span.min > 0 || imm_unlikely(src > dst)) continue;

        imm_float v = calc_score(mt, tt, 0, src, dst, 0, i);
        if (v > x.score) best_trans_set(&x, v, src, 0, i, 0);
    }

    return x;
}
