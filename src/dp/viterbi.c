#include "dp/viterbi.h"
#include "dp/best_trans.h"
#include "dp/emis.h"
#include "dp/hot_range.h"
#include "dp/matrix.h"
#include "dp/minmax.h"
#include "dp/path.h"
#include "dp/state_table.h"
#include "dp/trans_table.h"
#include "dp/unsafe_pair.h"
#include "eseq.h"
#include "imm/dp.h"
#include "task.h"

static inline void set_score(struct imm_dp const *dp, struct imm_task *task,
                             imm_float trans_score, unsigned min_len,
                             unsigned max_len, unsigned row, unsigned state)
{
    assume(max_len <= IMM_STATE_MAX_SEQLEN);
    for (unsigned len = min_len; len <= max_len; ++len)
    {
        unsigned seq_code = eseq_get(&task->eseq, row, len, min_len);
        imm_float score = trans_score + emis_score(&dp->emis, state, seq_code);
        matrix_set_score(&task->matrix, row, state, len, score);
    }
}

static inline void _viti_safe_future(struct imm_dp const *dp,
                                     struct imm_task *task, unsigned const r,
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

    set_score(dp, task, bt.score, span.min, span.max, r, i);
}

static inline void _viti_safe_future_nopath(struct imm_dp const *dp,
                                            struct imm_task *task,
                                            unsigned const r, unsigned i)
{
    struct best_trans bt = best_trans_find(dp, &task->matrix, i, r);
    struct span span = state_table_span(&dp->state_table, i);
    set_score(dp, task, bt.score, span.min, span.max, r, i);
}

static inline void _viti_safe_future_ge1(struct imm_dp const *dp,
                                         struct imm_task *task,
                                         unsigned const r, unsigned i)
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

    set_score(dp, task, bt.score, span.min, span.max, r, i);
}

static inline void _viti_safe_future_ge1_nopath(struct imm_dp const *dp,
                                                struct imm_task *task,
                                                unsigned const r, unsigned i)
{
    struct best_trans bt = best_trans_find_ge1(dp, &task->matrix, i, r);
    struct span span = state_table_span(&dp->state_table, i);
    set_score(dp, task, bt.score, span.min, span.max, r, i);
}

static inline void _viti_safe_ge1(struct imm_dp const *dp,
                                  struct imm_task *task, unsigned const r,
                                  unsigned i)
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
    set_score(dp, task, bt.score, span.min, span.max, r, i);
}

static inline void _viti_safe_ge1_nopath(struct imm_dp const *dp,
                                         struct imm_task *task,
                                         unsigned const r, unsigned i)
{
    struct best_trans bt = best_trans_find_safe_ge1(dp, &task->matrix, i, r);
    struct span span = state_table_span(&dp->state_table, i);
    set_score(dp, task, bt.score, span.min, span.max, r, i);
}

static inline void _viti_safe(struct imm_dp const *dp, struct imm_task *task,
                              unsigned const r, unsigned i)
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
    set_score(dp, task, bt.score, span.min, span.max, r, i);
}

static inline void _viti_safe_nopath(struct imm_dp const *dp,
                                     struct imm_task *task, unsigned const r,
                                     unsigned i)
{
    struct best_trans bt = best_trans_find_safe(dp, &task->matrix, i, r);
    struct span span = state_table_span(&dp->state_table, i);
    set_score(dp, task, bt.score, span.min, span.max, r, i);
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

static inline void _viti(struct imm_dp const *dp, struct imm_task *task,
                         unsigned const r, unsigned i, unsigned remain)
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
    span.max = MIN(span.max, remain);

    set_score(dp, task, bt.score, span.min, span.max, r, i);
}

static inline void _viti_nopath(struct imm_dp const *dp, struct imm_task *task,
                                unsigned const r, unsigned i, unsigned remain)
{
    struct best_trans bt = best_trans_find(dp, &task->matrix, i, r);
    struct span span = state_table_span(&dp->state_table, i);
    span.max = MIN(span.max, remain);
    set_score(dp, task, bt.score, span.min, span.max, r, i);
}

static inline void _viti_ge1(struct imm_dp const *dp, struct imm_task *task,
                             unsigned const r, unsigned i, unsigned remain)
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
    span.max = MIN(span.max, remain);

    set_score(dp, task, bt.score, span.min, span.max, r, i);
}

static inline void _viti_ge1_nopath(struct imm_dp const *dp,
                                    struct imm_task *task, unsigned const r,
                                    unsigned i, unsigned remain)
{
    struct best_trans bt = best_trans_find_ge1(dp, &task->matrix, i, r);
    struct span span = state_table_span(&dp->state_table, i);
    span.max = MIN(span.max, remain);
    set_score(dp, task, bt.score, span.min, span.max, r, i);
}

void viterbi_unsafe(struct imm_dp const *dp, struct imm_task *task,
                    unsigned const start_row, unsigned const stop_row,
                    unsigned seqlen, unsigned unsafe_state)
{
    for (unsigned r = start_row; r <= stop_row; ++r)
    {
        if ((r > 0 && r < seqlen))
        // if (r > 0 && upair)
        {
            _viti_ge1(dp, task, r, unsafe_state, stop_row - r);
        }
        for (unsigned i = 0; i < dp->state_table.nstates; ++i)
        {
            _viti(dp, task, r, i, stop_row - r);
        }
    }
}

void viterbi_unsafe_nopath(struct imm_dp const *dp, struct imm_task *task,
                           unsigned const start_row, unsigned const stop_row,
                           unsigned seqlen, unsigned unsafe_state)
{
    for (unsigned r = start_row; r <= stop_row; ++r)
    {
        if ((r > 0 && r < seqlen))
        // if (r > 0 && upair)
        {
            _viti_ge1_nopath(dp, task, r, unsafe_state, stop_row - r);
        }
        for (unsigned i = 0; i < dp->state_table.nstates; ++i)
        {
            _viti_nopath(dp, task, r, i, stop_row - r);
        }
    }
}

void viterbi_safe_future(struct imm_dp const *dp, struct imm_task *task,
                         unsigned const start_row, unsigned const stop_row,
                         unsigned unsafe_state)
{
    assert(start_row > 0);
    for (unsigned r = start_row; r <= stop_row; ++r)
    {
        _viti_safe_future_ge1(dp, task, r, unsafe_state);
        for (unsigned i = 0; i < dp->state_table.nstates; ++i)
        {
            _viti_safe_future(dp, task, r, i);
        }
    }
}

void viterbi_safe_future_nopath(struct imm_dp const *dp, struct imm_task *task,
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

void viterbi_safe(struct imm_dp const *dp, struct imm_task *task,
                  unsigned const start_row, unsigned const stop_row,
                  unsigned unsafe_state)
{
    assume(start_row > 0);
    for (unsigned r = start_row; r <= stop_row; ++r)
    {
        _viti_safe_ge1(dp, task, r, unsafe_state);
        for (unsigned i = 0; i < dp->state_table.nstates; ++i)
        {
            _viti_safe(dp, task, r, i);
        }
    }
}

#if 0
    printf("CUT-BEGIN\n");
    for (unsigned dst = 0; dst < dp->state_table.nstates; ++dst)
    {
        for (unsigned j = 0; j < trans_table_ntrans(&dp->trans_table, dst); ++j)
        {
            unsigned src = trans_table_source_state(&dp->trans_table, dst, j);
            struct span span = state_table_span(&dp->state_table, src);
            printf("[%d %d] ", span.min, span.max);
        }
        printf("\n");
    }
    printf("CUT-END\n");
#endif

void viterbi_safe_nopath(struct imm_dp const *dp, struct imm_task *task,
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
            bt.score = MAX(dp->state_table.start.lprob, bt.score);

        set_score(dp, task, bt.score, span.min, span.max, 0, i);
    }
}

void viterbi_row0_safe_nopath(struct imm_dp const *dp, struct imm_task *task)
{
    for (unsigned i = 0; i < dp->state_table.nstates; ++i)
    {
        struct best_trans bt = best_trans_find_row0(dp, &task->matrix, i);
        struct span span = state_table_span(&dp->state_table, i);

        if (dp->state_table.start.state == i)
            bt.score = MAX(dp->state_table.start.lprob, bt.score);

        set_score(dp, task, bt.score, span.min, span.max, 0, i);
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
        span.max = MIN(span.max, remain);

        if (dp->state_table.start.state == i)
            bt.score = MAX(dp->state_table.start.lprob, bt.score);

        set_score(dp, task, bt.score, span.min, span.max, 0, i);
    }
}
