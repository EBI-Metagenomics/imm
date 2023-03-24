#include "viterbi_path.h"
#include "best_trans.h"
#include "hot_range.h"
#include "imm/btrans.h"
#include "imm/dp.h"
#include "imm/range.h"
#include "imm/state_table.h"
#include "imm/trans_table.h"
#include "imm/viterbi.h"
#include "minmax.h"
#include "set_score.h"
#include "task.h"

static inline void set_cpath(struct cpath *path, struct imm_btrans const *bt,
                             unsigned r, unsigned i)
{
    if (bt->prev_state != IMM_STATE_NULL_IDX)
    {
        path_set_trans(path, r, i, bt->trans);
        path_set_seqlen(path, r, i, bt->len);
        assert(path_trans(path, r, i) == bt->trans);
        assert(path_seqlen(path, r, i) == bt->len);
    }
    else
    {
        path_invalidate(path, r, i);
        assert(!path_valid(path, r, i));
    }
}

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

static inline void safe_future(struct imm_viterbi const *x, unsigned r,
                               unsigned i)
{
    struct imm_btrans bt = imm_viterbi2_ge1(x, i, r);
    set_cpath(&x->task->path, &bt, r, i);
    set_score(x->dp, x->task, bt.score, r, i, safe_span(x->dp, i));
}

static inline void safe_future2(struct imm_viterbi const *x, unsigned r,
                                unsigned i)
{
    struct best_trans bt = best_trans_find(x->dp, &x->task->matrix, i, r);
    if (bt.prev_state != IMM_STATE_NULL_IDX)
    {
        path_set_trans(&x->task->path, r, i, bt.trans);
        path_set_seqlen(&x->task->path, r, i, bt.len);
        assert(path_trans(&x->task->path, r, i) == bt.trans);
        assert(path_seqlen(&x->task->path, r, i) == bt.len);
    }
    else
    {
        path_invalidate(&x->task->path, r, i);
        assert(!path_valid(&x->task->path, r, i));
    }

    // set_cpath(&x->task->path, &bt, r, i);
    set_score(x->dp, x->task, bt.score, r, i, safe_span(x->dp, i));
}

static inline void _viti_safe_ge1(struct imm_dp const *dp,
                                  struct imm_task *task, unsigned r, unsigned i)
{
    struct best_trans bt = best_trans_find_safe_ge1(dp, &task->matrix, i, r);
    if (bt.prev_state != IMM_STATE_NULL_IDX)
    {
        path_set_trans(&task->path, r, i, bt.trans);
        path_set_seqlen(&task->path, r, i, bt.len);
        assert(path_trans(&task->path, r, i) == bt.trans);
        assert(path_seqlen(&task->path, r, i) == bt.len);
    }
    else
    {
        path_invalidate(&task->path, r, i);
        assert(!path_valid(&task->path, r, i));
    }

    struct span span = imm_state_table_span(&dp->state_table, i);
    set_score(dp, task, bt.score, r, i, span);
}

static inline void _viti_safe(struct imm_dp const *dp, struct imm_task *task,
                              unsigned r, unsigned i)
{
    struct best_trans bt = best_trans_find_safe(dp, &task->matrix, i, r);
    if (bt.prev_state != IMM_STATE_NULL_IDX)
    {
        path_set_trans(&task->path, r, i, bt.trans);
        path_set_seqlen(&task->path, r, i, bt.len);
        assert(path_trans(&task->path, r, i) == bt.trans);
        assert(path_seqlen(&task->path, r, i) == bt.len);
    }
    else
    {
        path_invalidate(&task->path, r, i);
        assert(!path_valid(&task->path, r, i));
    }

    struct span span = imm_state_table_span(&dp->state_table, i);
    set_score(dp, task, bt.score, r, i, span);
}

static inline void _viti(struct imm_dp const *dp, struct imm_task *task,
                         unsigned r, unsigned i, unsigned remain)
{
    struct best_trans bt = best_trans_find(dp, &task->matrix, i, r);
    if (bt.prev_state != IMM_STATE_NULL_IDX)
    {
        path_set_trans(&task->path, r, i, bt.trans);
        path_set_seqlen(&task->path, r, i, bt.len);
        assert(path_trans(&task->path, r, i) == bt.trans);
        assert(path_seqlen(&task->path, r, i) == bt.len);
    }
    else
    {
        path_invalidate(&task->path, r, i);
        assert(!path_valid(&task->path, r, i));
    }

    struct span span = imm_state_table_span(&dp->state_table, i);
    span.max = min(span.max, remain);

    set_score(dp, task, bt.score, r, i, span);
}

static inline void unsafe_ge1(struct imm_viterbi const *x,
                              struct imm_range const *range, unsigned r,
                              unsigned i)
{
    struct imm_btrans bt = imm_viterbi2_ge1(x, i, r);
    set_cpath(&x->task->path, &bt, r, i);
    unsigned rem = range->b - r - 1;
    set_score(x->dp, x->task, bt.score, r, i, unsafe_span(x->dp, i, rem));
}

static inline void unsafe(struct imm_viterbi const *x,
                          struct imm_range const *range, unsigned r, unsigned i)
{
    struct imm_btrans bt = imm_viterbi2(x, i, r);
    set_cpath(&x->task->path, &bt, r, i);
    unsigned rem = range->b - r - 1;
    set_score(x->dp, x->task, bt.score, r, i, unsafe_span(x->dp, i, rem));
}

static inline void _viti_ge1(struct imm_dp const *dp, struct imm_task *task,
                             unsigned r, unsigned i, unsigned remain)
{
    struct best_trans bt = best_trans_find_ge1(dp, &task->matrix, i, r);
    if (bt.prev_state != IMM_STATE_NULL_IDX)
    {
        path_set_trans(&task->path, r, i, bt.trans);
        path_set_seqlen(&task->path, r, i, bt.len);
        assert(path_trans(&task->path, r, i) == bt.trans);
        assert(path_seqlen(&task->path, r, i) == bt.len);
    }
    else
    {
        path_invalidate(&task->path, r, i);
        assert(!path_valid(&task->path, r, i));
    }

    struct span span = imm_state_table_span(&dp->state_table, i);
    span.max = min(span.max, remain);

    set_score(dp, task, bt.score, r, i, span);
}

// Assume: `x->a > 0`.
void viterbi_path_unsafe(struct imm_viterbi const *x,
                         struct imm_range const *range, unsigned len)
{
    assume(range->a > 0);

    for (unsigned r = range->a; r < range->b; ++r)
    {
        if (r < len) unsafe(x, range, r, x->unsafe_state);
        for (unsigned i = 0; i < imm_dp_nstates(x->dp); ++i)
            unsafe(x, range, r, i);
    }
}

// Assume: `seqlen >= max(max_seq) + row` and `row > 0`.
void viterbi_path_safe_past(struct imm_viterbi const *x,
                            struct imm_range const *range)
{
    assume(range->a > 0);

    for (unsigned r = range->a; r < range->b; ++r)
    {
        unsafe(x, range, r, x->unsafe_state);
        for (unsigned i = 0; i < imm_dp_nstates(x->dp); ++i)
            unsafe(x, range, r, i);
    }
}

void viterbi_path_row0(struct imm_viterbi const *x, unsigned remain)
{
    for (unsigned i = 0; i < imm_dp_nstates(x->dp); ++i)
    {
        struct best_trans bt = best_trans_find_row0(x->dp, &x->task->matrix, i);
        if (bt.prev_state != IMM_STATE_NULL_IDX)
        {
            path_set_trans(&x->task->path, 0, i, bt.trans);
            path_set_seqlen(&x->task->path, 0, i, bt.len);
            assert(path_trans(&x->task->path, 0, i) == bt.trans);
            assert(path_seqlen(&x->task->path, 0, i) == bt.len);
        }
        else
        {
            path_invalidate(&x->task->path, 0, i);
            assert(!path_valid(&x->task->path, 0, i));
        }

        struct span span = imm_state_table_span(&x->dp->state_table, i);
        span.max = min(span.max, remain);

        if (x->dp->state_table.start.state == i)
            bt.score = max(x->dp->state_table.start.lprob, bt.score);

        set_score(x->dp, x->task, bt.score, 0, i, span);
    }
}

void viterbi_path_safe_future(struct imm_viterbi const *x,
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

void viterbi_path_safe(struct imm_viterbi const *x,
                       struct imm_range const *range)
{
    assume(range->a > 0);
    for (unsigned r = range->a; r < range->b; ++r)
    {
        _viti_safe_ge1(x->dp, x->task, r, x->unsafe_state);
        for (unsigned i = 0; i < x->dp->state_table.nstates; ++i)
        {
            _viti_safe(x->dp, x->task, r, i);
        }
    }
}

void viterbi_path_safe_row0(struct imm_viterbi const *x)
{
    for (unsigned i = 0; i < imm_dp_nstates(x->dp); ++i)
    {
        struct imm_btrans bt = imm_viterbi2_row0(x, i);

        if (x->dp->state_table.start.state == i)
            bt.score = max(x->dp->state_table.start.lprob, bt.score);

        set_cpath(&x->task->path, &bt, 0, i);
        set_score(x->dp, x->task, bt.score, 0, i, safe_span(x->dp, i));
    }
}
