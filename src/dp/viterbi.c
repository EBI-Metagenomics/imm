#include "dp/viterbi.h"
#include "dp/best_trans.h"
#include "dp/hot_range.h"
#include "dp/set_score.h"
#include "dp/state_table.h"
#include "dp/trans_table.h"
#include "imm/dp.h"
#include "minmax.h"
#include "range.h"
#include "task.h"

static inline void _viti_safe_future(struct imm_dp const *dp,
                                     struct imm_task *task, unsigned r,
                                     unsigned i)
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

    struct span span = state_table_span(&dp->state_table, i);

    set_score(dp, task, bt.score, r, i, span);
}

static inline void _viti_safe_future_ge1(struct imm_dp const *dp,
                                         struct imm_task *task, unsigned r,
                                         unsigned i)
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

    struct span span = state_table_span(&dp->state_table, i);

    set_score(dp, task, bt.score, r, i, span);
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

    struct span span = state_table_span(&dp->state_table, i);
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

    struct span span = state_table_span(&dp->state_table, i);
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

    struct span span = state_table_span(&dp->state_table, i);
    span.max = min(span.max, remain);

    set_score(dp, task, bt.score, r, i, span);
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

    struct span span = state_table_span(&dp->state_table, i);
    span.max = min(span.max, remain);

    set_score(dp, task, bt.score, r, i, span);
}

void viterbi_unsafe(struct imm_dp const *dp, struct imm_task *task,
                    struct imm_range const *range, unsigned len,
                    unsigned unsafe_state)
{
    for (unsigned r = range->a; r < range->b; ++r)
    {
        if ((r > 0 && r < len))
            _viti_ge1(dp, task, r, unsafe_state, (unsigned)(range->b - r - 1));

        for (unsigned i = 0; i < dp->state_table.nstates; ++i)
            _viti(dp, task, r, i, (unsigned)(range->b - r - 1));
    }
}

void viterbi_safe_future(struct imm_dp const *dp, struct imm_task *task,
                         struct imm_range const *range, unsigned unsafe_state)
{
    assert(range->a > 0);
    for (unsigned r = range->a; r < range->b; ++r)
    {
        _viti_safe_future_ge1(dp, task, r, unsafe_state);
        for (unsigned i = 0; i < dp->state_table.nstates; ++i)
        {
            _viti_safe_future(dp, task, r, i);
        }
    }
}

void viterbi_safe(struct imm_dp const *dp, struct imm_task *task,
                  struct imm_range const *range, unsigned unsafe_state)
{
    assume(range->a > 0);
    for (unsigned r = range->a; r < range->b; ++r)
    {
        _viti_safe_ge1(dp, task, r, unsafe_state);
        for (unsigned i = 0; i < dp->state_table.nstates; ++i)
        {
            _viti_safe(dp, task, r, i);
        }
    }
}

void viterbi_row0_safe(struct imm_dp const *dp, struct imm_task *task)
{
    for (unsigned i = 0; i < dp->state_table.nstates; ++i)
    {
        struct best_trans bt = best_trans_find_row0(dp, &task->matrix, i);
        if (bt.prev_state != IMM_STATE_NULL_IDX)
        {
            path_set_trans(&task->path, 0, i, bt.trans);
            path_set_seqlen(&task->path, 0, i, bt.len);
            assert(path_trans(&task->path, 0, i) == bt.trans);
            assert(path_seqlen(&task->path, 0, i) == bt.len);
        }
        else
        {
            path_invalidate(&task->path, 0, i);
            assert(!path_valid(&task->path, 0, i));
        }

        struct span span = state_table_span(&dp->state_table, i);

        if (dp->state_table.start.state == i)
            bt.score = max(dp->state_table.start.lprob, bt.score);

        set_score(dp, task, bt.score, 0, i, span);
    }
}

void viterbi_row0_safe_nopath(struct imm_dp const *dp, struct imm_task *task)
{
    for (unsigned i = 0; i < dp->state_table.nstates; ++i)
    {
        struct best_trans bt = best_trans_find_row0(dp, &task->matrix, i);
        struct span span = state_table_span(&dp->state_table, i);

        if (dp->state_table.start.state == i)
            bt.score = max(dp->state_table.start.lprob, bt.score);

        set_score(dp, task, bt.score, 0, i, span);
    }
}

void viterbi_row0(struct imm_dp const *dp, struct imm_task *task,
                  unsigned remain)
{
    for (unsigned i = 0; i < dp->state_table.nstates; ++i)
    {
        struct best_trans bt = best_trans_find_row0(dp, &task->matrix, i);
        if (bt.prev_state != IMM_STATE_NULL_IDX)
        {
            path_set_trans(&task->path, 0, i, bt.trans);
            path_set_seqlen(&task->path, 0, i, bt.len);
            assert(path_trans(&task->path, 0, i) == bt.trans);
            assert(path_seqlen(&task->path, 0, i) == bt.len);
        }
        else
        {
            path_invalidate(&task->path, 0, i);
            assert(!path_valid(&task->path, 0, i));
        }

        struct span span = state_table_span(&dp->state_table, i);
        span.max = min(span.max, remain);

        if (dp->state_table.start.state == i)
            bt.score = max(dp->state_table.start.lprob, bt.score);

        set_score(dp, task, bt.score, 0, i, span);
    }
}
