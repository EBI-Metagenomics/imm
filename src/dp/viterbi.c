#include "dp/viterbi.h"
#include "dp/best_trans.h"
#include "dp/emis.h"
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

    unsigned min_len = state_table_span(&dp->state_table, i).min;
    unsigned max_len = state_table_span(&dp->state_table, i).max;

    set_score(dp, task, bt.score, min_len, max_len, r, i);
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

    unsigned min_len = state_table_span(&dp->state_table, i).min;
    unsigned max_len =
        (unsigned)MIN(state_table_span(&dp->state_table, i).max, remain);

    set_score(dp, task, bt.score, min_len, max_len, r, i);
}

void viterbi_unsafe(struct imm_dp const *dp, struct imm_task *task,
                    unsigned const start_row, unsigned const stop_row,
                    unsigned seqlen, struct unsafe_pair *upair)
{
    for (unsigned r = start_row; r <= stop_row; ++r)
    {
        if ((r > 0 && r < seqlen) && upair)
        // if (r > 0 && upair)
        {
            // printf("-----\n");
            // imm_matrix_dump(&task->matrix, stdout);
            _viti(dp, task, r, upair->src, stop_row - r);
            // printf("-----\n");
            // imm_matrix_dump(&task->matrix, stdout);
        }
        for (unsigned i = 0; i < dp->state_table.nstates; ++i)
        {
            _viti(dp, task, r, i, stop_row - r);
        }
    }
}

void viterbi_safe_future(struct imm_dp const *dp, struct imm_task *task,
                         unsigned const start_row, unsigned const stop_row,
                         struct unsafe_pair *upair)
{
    for (unsigned r = start_row; r <= stop_row; ++r)
    {
        if (r > 0 && upair)
        {
            // printf("-----\n");
            // imm_matrix_dump(&task->matrix, stdout);
            _viti_safe_future(dp, task, r, upair->src);
            // printf("-----\n");
            // imm_matrix_dump(&task->matrix, stdout);
        }
        for (unsigned i = 0; i < dp->state_table.nstates; ++i)
        {
            _viti_safe_future(dp, task, r, i);
        }
    }
}

void viterbi_safe(struct imm_dp const *dp, struct imm_task *task,
                  unsigned const start_row, unsigned const stop_row,
                  struct unsafe_pair *upair)
{
    assume(start_row > 0);
    for (unsigned r = start_row; r <= stop_row; ++r)
    {
        if (upair)
        {
            // printf("-----\n");
            // imm_matrix_dump(&task->matrix, stdout);
            _viti_safe(dp, task, r, upair->src);
            // printf("-----\n");
            // imm_matrix_dump(&task->matrix, stdout);
        }
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

        unsigned min_len = state_table_span(&dp->state_table, i).min;
        unsigned max_len = state_table_span(&dp->state_table, i).max;

        if (dp->state_table.start.state == i)
            bt.score = MAX(dp->state_table.start.lprob, bt.score);

        set_score(dp, task, bt.score, min_len, max_len, 0, i);
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

        unsigned min_len = state_table_span(&dp->state_table, i).min;
        unsigned max_len =
            (unsigned)MIN(state_table_span(&dp->state_table, i).max, remain);

        if (dp->state_table.start.state == i)
            bt.score = MAX(dp->state_table.start.lprob, bt.score);

        set_score(dp, task, bt.score, min_len, max_len, 0, i);
    }
}
