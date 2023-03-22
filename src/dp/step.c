#include "dp/step.h"
#include "dp/matrix.h"
#include "dp/state_table.h"
#include "dp/trans_table.h"
#include "imm/float.h"
#include "minmax.h"

static inline unsigned ntrans(struct imm_dp_step const *x, unsigned dst)
{
    return trans_table_ntrans(x->trans_table, dst);
}

static inline unsigned source_state(struct imm_dp_step const *x, unsigned dst,
                                    unsigned trans)
{
    return trans_table_source_state(x->trans_table, dst, trans);
}

static inline struct span state_span(struct imm_dp_step const *x, unsigned src)
{
    return state_table_span(x->state_table, src);
}

static inline imm_float total_score(struct imm_dp_step const *x, unsigned row,
                                    unsigned src, unsigned dst, unsigned len,
                                    unsigned trans)
{
    imm_float v0 = matrix_get_score(x->score, cell_init(row, src, len));
    imm_float v1 = trans_table_score(x->trans_table, dst, trans);
    return v0 + v1;
}

imm_float imm_dp_step_score(struct imm_dp_step const *x, unsigned dst,
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
            score = score < v ? v : score;
        }
    }
    return score;
}

imm_float imm_dp_step_score_ge1(struct imm_dp_step const *x, unsigned dst,
                                unsigned row)
{
    imm_float score = imm_lprob_zero();

    for (unsigned i = 0; i < ntrans(x, dst); ++i)
    {
        unsigned src = source_state(x, dst, i);
        struct span span = state_span(x, src);
        if (span.min == 0) continue;

        assume(span.min >= 0 && row >= span.min && row >= span.max);

        for (unsigned len = span.min; len <= span.max; ++len)
        {
            assume(row >= len && len >= span.min);
            imm_float v = total_score(x, row - len, src, dst, len, i);
            score = score < v ? v : score;
        }
    }
    return score;
}

imm_float imm_dp_step_score_safe(struct imm_dp_step const *x, unsigned dst,
                                 unsigned row)
{
    imm_float score = imm_lprob_zero();

    for (unsigned i = 0; i < ntrans(x, dst); ++i)
    {
        unsigned src = source_state(x, dst, i);
        struct span span = state_span(x, src);
        assume(span.min >= 0 && row >= span.min && row >= span.max);
        for (unsigned len = span.min; len <= span.max; ++len)
        {
            assume(row >= len && len >= span.min);
            imm_float v = total_score(x, row - len, src, dst, len, i);
            score = score < v ? v : score;
        }
    }
    return score;
}

imm_float imm_dp_step_score_row0(struct imm_dp_step const *x, unsigned dst)
{
    imm_float score = imm_lprob_zero();

    for (unsigned i = 0; i < ntrans(x, dst); ++i)
    {
        unsigned src = source_state(x, dst, i);
        struct span span = state_span(x, src);

        if (span.min > 0 || imm_unlikely(src > dst)) continue;

        imm_float v = total_score(x, 0, src, dst, 0, i);
        score = score < v ? v : score;
    }
    return score;
}

imm_float imm_dp_step_score_safe_ge1(struct imm_dp_step const *x, unsigned dst,
                                     unsigned row)
{
    imm_float score = imm_lprob_zero();

    for (unsigned i = 0; i < ntrans(x, dst); ++i)
    {
        unsigned src = source_state(x, dst, i);
        struct span span = state_span(x, src);
        if (span.min == 0) continue;

        assume(span.min >= 0 && row >= span.min && row >= span.max);

        for (unsigned len = span.min; len <= span.max; ++len)
        {
            assume(row >= len && len >= span.min);
            imm_float v = total_score(x, row - len, src, dst, len, i);
            score = score < v ? v : score;
        }
    }
    return score;
}
