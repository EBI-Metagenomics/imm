#include "dp/viterbi_nopath.h"
#include "dp/best_trans_score.h"
#include "dp/hot_range.h"
#include "dp/set_score.h"
#include "dp/state_table.h"
#include "dp/trans_table.h"
#include "imm/dp.h"
#include "minmax.h"
#include "task.h"

static inline void _viti_ge1_nopath(struct imm_dp const *dp,
                                    struct imm_task *task, unsigned const r,
                                    unsigned i, unsigned remain)
{
    imm_float score = best_trans_score_find_ge1(dp, &task->matrix, i, r);
    struct span span = state_table_span(&dp->state_table, i);
    span.max = min(span.max, remain);
    set_score(dp, task, score, span.min, span.max, r, i);
}

static inline void _viti_nopath(struct imm_dp const *dp, struct imm_task *task,
                                unsigned const r, unsigned i, unsigned remain)
{
    imm_float score = best_trans_score_find(dp, &task->matrix, i, r);
    struct span span = state_table_span(&dp->state_table, i);
    span.max = min(span.max, remain);
    set_score(dp, task, score, span.min, span.max, r, i);
}

void viterbi_nopath_unsafe(struct imm_dp const *dp, struct imm_task *task,
                           unsigned const start_row, unsigned const stop_row,
                           unsigned seqlen, unsigned unsafe_state)
{
    for (unsigned r = start_row; r <= stop_row; ++r)
    {
        if ((r > 0 && r < seqlen))
        {
            _viti_ge1_nopath(dp, task, r, unsafe_state, stop_row - r);
        }
        for (unsigned i = 0; i < dp->state_table.nstates; ++i)
        {
            _viti_nopath(dp, task, r, i, stop_row - r);
        }
    }
}

static inline void _viti_safe_future_nopath(struct imm_dp const *dp,
                                            struct imm_task *task,
                                            unsigned const r, unsigned i)
{
    imm_float score = best_trans_score_find(dp, &task->matrix, i, r);
    struct span span = state_table_span(&dp->state_table, i);
    set_score(dp, task, score, span.min, span.max, r, i);
}

static inline void _viti_safe_future_ge1_nopath(struct imm_dp const *dp,
                                                struct imm_task *task,
                                                unsigned const r, unsigned i)
{
    imm_float score = best_trans_score_find_ge1(dp, &task->matrix, i, r);
    struct span span = state_table_span(&dp->state_table, i);
    set_score(dp, task, score, span.min, span.max, r, i);
}

void viterbi_nopath_safe_future(struct imm_dp const *dp, struct imm_task *task,
                                unsigned const start_row,
                                unsigned const stop_row, unsigned unsafe_state)
{
    assert(start_row > 0);
    for (unsigned r = start_row; r <= stop_row; ++r)
    {
        _viti_safe_future_ge1_nopath(dp, task, r, unsafe_state);
        for (unsigned i = 0; i < dp->state_table.nstates; ++i)
        {
            _viti_safe_future_nopath(dp, task, r, i);
        }
    }
}

static inline void _viti_safe_ge1_nopath(struct imm_dp const *dp,
                                         struct imm_task *task,
                                         unsigned const r, unsigned i)
{
    imm_float score = best_trans_score_find_safe_ge1(dp, &task->matrix, i, r);
    struct span span = state_table_span(&dp->state_table, i);
    set_score(dp, task, score, span.min, span.max, r, i);
}

static inline void _viti_safe_nopath(struct imm_dp const *dp,
                                     struct imm_task *task, unsigned const r,
                                     unsigned i)
{
    imm_float score = best_trans_score_find_safe(dp, &task->matrix, i, r);
    struct span span = state_table_span(&dp->state_table, i);
    set_score(dp, task, score, span.min, span.max, r, i);
}

static inline void _viti_safe_hot_nopath(struct imm_dp const *dp,
                                         struct imm_task *task,
                                         unsigned const row, unsigned dst,
                                         struct hot_range const *hot)
{

    struct imm_dp_trans_table const *tt = &dp->trans_table;
    struct matrix const *mt = &task->matrix;

    imm_float v0 = matrix_get_score(mt, row - 1, dst - 1, 1) +
                   trans_table_score(tt, dst, 0);
    imm_float v1 =
        matrix_get_score(mt, row, hot->left, 0) + trans_table_score(tt, dst, 1);

    set_score(dp, task, v0 < v1 ? v1 : v0, 1, 1, row, dst);
}

void viterbi_nopath_safe(struct imm_dp const *dp, struct imm_task *task,
                         unsigned const start_row, unsigned const stop_row,
                         unsigned unsafe_state)
{
    struct hot_range hot = {0};
    imm_hot_range(dp, (struct span){1, 1}, &hot);

    assume(start_row > 0);
    for (unsigned r = start_row; r <= stop_row; ++r)
    {
        _viti_safe_ge1_nopath(dp, task, r, unsafe_state);

        if (hot.total < 2)
        {
            for (unsigned i = 0; i < dp->state_table.nstates; ++i)
                _viti_safe_nopath(dp, task, r, i);
        }
        else
        {
            for (unsigned i = 0; i < hot.start; ++i)
                _viti_safe_nopath(dp, task, r, i);

            for (unsigned i = hot.start; i < hot.start + hot.total; ++i)
            {
                // _viti_safe_nopath(dp, task, r, i);
                _viti_safe_hot_nopath(dp, task, r, i, &hot);
            }

            for (unsigned i = hot.start + hot.total;
                 i < dp->state_table.nstates; ++i)
                _viti_safe_nopath(dp, task, r, i);
        }
    }
}
