#include "viterbi_nopath.h"
#include "hot_range.h"
#include "imm/dp.h"
#include "imm/viterbi.h"
#include "minmax.h"
#include "range.h"
#include "set_score.h"
#include "task.h"

static inline struct span safe_span(struct imm_dp const *dp, unsigned state)
{
    return imm_state_table_span(&dp->state_table, state);
}

static inline struct span unsafe_span(struct imm_dp const *dp, unsigned state,
                                      unsigned max)
{
    struct span x = imm_state_table_span(&dp->state_table, state);
    return span_init(x.min, x.max <= max ? x.max : max);
}

static inline void unsafe_row0(struct imm_viterbi const *x, unsigned i)
{
    imm_float score = imm_viterbi_score(x, i, 0);
    if (x->dp->state_table.start.state == i)
        score = max(x->dp->state_table.start.lprob, score);
    set_score(x->dp, x->task, score, 0, i, unsafe_span(x->dp, i, 0));
}

static inline void unsafe(struct imm_viterbi const *x,
                          struct imm_range const *range, unsigned r, unsigned i)
{
    imm_float score = imm_viterbi_score(x, i, r);
    unsigned rem = range->b - r - 1;
    set_score(x->dp, x->task, score, r, i, unsafe_span(x->dp, i, rem));
}

static inline void safe_future(struct imm_viterbi const *x, unsigned r,
                               unsigned i)
{
    imm_float score = imm_viterbi_score_safe_future(x, i, r);
    set_score(x->dp, x->task, score, r, i, safe_span(x->dp, i));
}

static inline void safe_future2(struct imm_viterbi const *x, unsigned r,
                                unsigned i)
{
    imm_float score = imm_viterbi_score(x, i, r);
    set_score(x->dp, x->task, score, r, i, safe_span(x->dp, i));
}

static inline void safe(struct imm_viterbi const *x, unsigned r, unsigned i)
{
    imm_float score = imm_viterbi_score_safe(x, i, r);
    set_score(x->dp, x->task, score, r, i, safe_span(x->dp, i));
}

static inline void safe_row0(struct imm_viterbi const *x, unsigned i)
{
    imm_float score = imm_viterbi_score_row0(x, i);
    if (x->dp->state_table.start.state == i)
        score = max(x->dp->state_table.start.lprob, score);
    set_score(x->dp, x->task, score, 0, i, safe_span(x->dp, i));
}

static inline void safe_hot(struct imm_dp const *dp, struct imm_task *task,
                            unsigned row, unsigned dst,
                            struct hot_range const *hot)
{
    struct imm_matrix const *mt = &task->matrix;

    imm_float v0 =
        imm_matrix_get_score(mt, imm_cell_init(row - 1, dst - 1, 1)) +
        imm_trans_table_score(&dp->trans_table, dst, 0);
    imm_float v1 = imm_matrix_get_score(mt, imm_cell_init(row, hot->left, 0)) +
                   imm_trans_table_score(&dp->trans_table, dst, 1);

    imm_float score = v0 < v1 ? v1 : v0;
    set_score(dp, task, score, row, dst, span_init(1, 1));
}

// Assume: `x->a > 0`.
void viterbi_nopath(struct imm_viterbi const *x, struct imm_range const *range)
{
    assume(range->a > 0);

    for (unsigned r = range->a; r < range->b; ++r)
    {
        unsafe(x, range, r, x->unsafe_state);
        for (unsigned i = 0; i < imm_dp_nstates(x->dp); ++i)
            unsafe(x, range, r, i);
    }
}

// Assume: `row == 0`.
void viterbi_nopath_row0(struct imm_viterbi const *x)
{
    unsafe_row0(x, x->unsafe_state);
    for (unsigned i = 0; i < imm_dp_nstates(x->dp); ++i)
        unsafe_row0(x, i);
}

// Assume: `seqlen >= max(max_seq) + row and row > 0`.
void viterbi_nopath_safe_future(struct imm_viterbi const *x,
                                struct imm_range const *range)
{
    assert(range->a > 0);

    for (unsigned r = range->a; r < range->b; ++r)
    {
        safe_future(x, r, x->unsafe_state);
        for (unsigned i = 0; i < imm_dp_nstates(x->dp); ++i)
            safe_future2(x, r, i);
    }
}

// Assume: seqlen >= max(max_seq) + row, row >= max(max_seq) > 0, and row > 0.
void viterbi_nopath_safe(struct imm_viterbi const *x,
                         struct imm_range const *range)
{
    struct hot_range hot = {0};
    imm_hot_range(x->dp, (struct span){1, 1}, &hot);
    assume(range->a > 0);

    for (unsigned r = range->a; r < range->b; ++r)
    {
        unsigned state = x->unsafe_state;
        imm_float score = imm_viterbi_score_safe(x, state, r);
        set_score(x->dp, x->task, score, r, state, safe_span(x->dp, state));

        for (unsigned i = 0; i < hot.start; ++i)
            safe(x, r, i);

        for (unsigned i = hot.start; i < hot.start + hot.total; ++i)
            safe_hot(x->dp, x->task, r, i, &hot);

        for (unsigned i = hot.start + hot.total; i < imm_dp_nstates(x->dp); ++i)
            safe(x, r, i);
    }
}

void viterbi_nopath_safe_row0(struct imm_viterbi const *x)
{
    safe_row0(x, x->unsafe_state);
    for (unsigned i = 0; i < imm_dp_nstates(x->dp); ++i)
        safe_row0(x, i);
}
