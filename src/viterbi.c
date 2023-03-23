#include "imm/viterbi.h"
#include "assume.h"
#include "imm/btrans.h"
#include "imm/cell.h"
#include "imm/dp.h"
#include "imm/float.h"
#include "imm/matrix.h"
#include "imm/state_table.h"
#include "imm/trans_table.h"
#include "minmax.h"
#include "span.h"
#include "task.h"

static inline unsigned ntrans(struct imm_viterbi const *x, unsigned dst)
{
    return imm_trans_table_ntrans(&x->dp->trans_table, dst);
}

static inline unsigned source_state(struct imm_viterbi const *x, unsigned dst,
                                    unsigned trans)
{
    return imm_trans_table_source_state(&x->dp->trans_table, dst, trans);
}

static inline struct span state_span(struct imm_viterbi const *x, unsigned src)
{
    return imm_state_table_span(&x->dp->state_table, src);
}

static inline imm_float total_score(struct imm_viterbi const *x, unsigned row,
                                    unsigned src, unsigned dst, unsigned len,
                                    unsigned trans)
{
    imm_float v0 =
        imm_matrix_get_score(&x->task->matrix, imm_cell_init(row, src, len));
    imm_float v1 = imm_trans_table_score(&x->dp->trans_table, dst, trans);
    return v0 + v1;
}

imm_float imm_viterbi_score(struct imm_viterbi const *x, unsigned dst,
                            unsigned row)
{
    imm_float score = imm_lprob_zero();

    for (unsigned i = 0; i < ntrans(x, dst); ++i)
    {
        unsigned src = source_state(x, dst, i);
        struct span span = state_span(x, src);

        if (imm_unlikely(row < span.min)) continue;

        span.max = min(span.max, row);
        for (unsigned len = span.min; len <= span.max; ++len)
        {
            assume(row >= len && len >= span.min);
            imm_float v = total_score(x, row - len, src, dst, len, i);
            score = max(score, v);
        }
    }
    return score;
}

// Assume: row == 0.
imm_float imm_viterbi_score_row0(struct imm_viterbi const *x, unsigned dst)
{
    imm_float score = imm_lprob_zero();

    for (unsigned i = 0; i < ntrans(x, dst); ++i)
    {
        unsigned src = source_state(x, dst, i);
        struct span span = state_span(x, src);

        if (span.min > 0 || imm_unlikely(src > dst)) continue;

        imm_float v = total_score(x, 0, src, dst, 0, i);
        score = max(score, v);
    }
    return score;
}

// Assume: seqlen >= max(max_seq) + row and row > 0. Therefore,
// we don't need `span.max = min(span.max, row);`, avoiding a conditional
// jump.
imm_float imm_viterbi_score_safe_future(struct imm_viterbi const *x,
                                        unsigned dst, unsigned row)
{
    imm_float score = imm_lprob_zero();

    for (unsigned i = 0; i < ntrans(x, dst); ++i)
    {
        unsigned src = source_state(x, dst, i);
        struct span span = state_span(x, src);

        assume(row >= span.min && row >= span.max);
        if (span.min == 0) continue;

        for (unsigned len = span.min; len <= span.max; ++len)
        {
            assume(row >= len && len >= span.min);
            imm_float v = total_score(x, row - len, src, dst, len, i);
            score = max(score, v);
        }
    }
    return score;
}

// Assume: seqlen >= max(max_seq) + row, row >= max(max_seq) > 0, and row > 0.
// Therefore, we don't need `span.max = min(span.max, row);` nor
// `if (span.min == 0) continue;`.
imm_float imm_viterbi_score_safe(struct imm_viterbi const *x, unsigned dst,
                                 unsigned row)
{
    imm_float score = imm_lprob_zero();

    for (unsigned i = 0; i < ntrans(x, dst); ++i)
    {
        unsigned src = source_state(x, dst, i);
        struct span span = state_span(x, src);

        assume(row >= span.min && row >= span.max);

        for (unsigned len = span.min; len <= span.max; ++len)
        {
            assume(row >= len && len >= span.min);
            imm_float v = total_score(x, row - len, src, dst, len, i);
            score = max(score, v);
        }
    }
    return score;
}

struct imm_btrans imm_viterbi2_ge1(struct imm_viterbi const *x, unsigned dst,
                                   unsigned row)
{
    struct imm_btrans bt = imm_btrans_init();

    for (unsigned i = 0; i < ntrans(x, dst); ++i)
    {
        unsigned src = source_state(x, dst, i);
        struct span span = state_span(x, src);
        if (span.min == 0) continue;

        if (imm_unlikely(row < span.min)) continue;

        span.max = min(span.max, row);
        for (unsigned len = span.min; len <= span.max; ++len)
        {
            assume(row >= len && len >= span.min);
            imm_float v = total_score(x, row - len, src, dst, len, i);
            if (v > bt.score) imm_btrans_set(&bt, v, src, i, len - span.min);
        }
    }
    return bt;
}

struct imm_btrans imm_viterbi2(struct imm_viterbi const *x, unsigned dst,
                               unsigned row)
{
    struct imm_btrans bt = imm_btrans_init();

    for (unsigned i = 0; i < ntrans(x, dst); ++i)
    {
        unsigned src = source_state(x, dst, i);
        struct span span = state_span(x, src);

        if (imm_unlikely(row < span.min)) continue;

        span.max = min(span.max, row);
        for (unsigned len = span.min; len <= span.max; ++len)
        {
            assume(row >= len && len >= span.min);
            imm_float v = total_score(x, row - len, src, dst, len, i);
            if (v > bt.score) imm_btrans_set(&bt, v, src, i, len - span.min);
        }
    }
    return bt;
}

// Assume: row == 0.
struct imm_btrans imm_viterbi2_row0(struct imm_viterbi const *x, unsigned dst)
{
    struct imm_btrans bt = imm_btrans_init();

    for (unsigned i = 0; i < ntrans(x, dst); ++i)
    {
        unsigned src = source_state(x, dst, i);
        struct span span = state_span(x, src);

        if (span.min > 0 || imm_unlikely(src > dst)) continue;

        imm_float v = total_score(x, 0, src, dst, 0, i);
        if (v > bt.score) imm_btrans_set(&bt, v, src, i, 0);
    }
    return bt;
}

// Assume: seqlen >= max(max_seq) + row and row > 0. Therefore,
// we don't need `span.max = min(span.max, row);`, avoiding a conditional
// jump.
struct imm_btrans imm_viterbi2_safe_future(struct imm_viterbi const *x,
                                           unsigned dst, unsigned row)
{
    struct imm_btrans bt = imm_btrans_init();

    for (unsigned i = 0; i < ntrans(x, dst); ++i)
    {
        unsigned src = source_state(x, dst, i);
        struct span span = state_span(x, src);

        assume(row >= span.min && row >= span.max);
        if (span.min == 0) continue;

        for (unsigned len = span.min; len <= span.max; ++len)
        {
            assume(row >= len && len >= span.min);
            imm_float v = total_score(x, row - len, src, dst, len, i);
            if (v > bt.score) imm_btrans_set(&bt, v, src, i, len - span.min);
        }
    }
    return bt;
}

// Assume: seqlen >= max(max_seq) + row, row >= max(max_seq) > 0, and row > 0.
// Therefore, we don't need `span.max = min(span.max, row);` nor
// `if (span.min == 0) continue;`.
struct imm_btrans imm_viterbi2_safe(struct imm_viterbi const *x, unsigned dst,
                                    unsigned row)
{
    struct imm_btrans bt = imm_btrans_init();

    for (unsigned i = 0; i < ntrans(x, dst); ++i)
    {
        unsigned src = source_state(x, dst, i);
        struct span span = state_span(x, src);

        assume(row >= span.min && row >= span.max);

        for (unsigned len = span.min; len <= span.max; ++len)
        {
            assume(row >= len && len >= span.min);
            imm_float v = total_score(x, row - len, src, dst, len, i);
            if (v > bt.score) imm_btrans_set(&bt, v, src, i, len - span.min);
        }
    }
    return bt;
}
