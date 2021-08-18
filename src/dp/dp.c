#include "dp/dp.h"
#include "dp/code.h"
#include "dp/emis.h"
#include "dp/matrix.h"
#include "dp/state_table.h"
#include "dp/trans_table.h"
#include "elapsed/elapsed.h"
#include "imm/dp.h"
#include "imm/lprob.h"
#include "imm/path.h"
#include "imm/result.h"
#include "imm/state.h"
#include "imm/support.h"
#include "imm/trans.h"
#include "support.h"
#include "task.h"

struct final_score
{
    imm_float score;
    unsigned state;
    unsigned seq_len;
};

static struct final_score best_trans_score(struct imm_dp const *dp,
                                           struct matrix const *matrix,
                                           unsigned tgt_state, unsigned row,
                                           uint16_t *best_trans,
                                           uint8_t *best_len);

static struct final_score
best_trans_score_first_row(struct imm_dp const *dp, struct matrix const *matrix,
                           unsigned tgt_state, uint16_t *best_trans,
                           uint8_t *best_len);

static struct final_score final_score(struct imm_dp const *dp,
                                      struct imm_task *task);

static unsigned max_seq(unsigned nstates, struct imm_state **states)
{
    unsigned max = imm_state_span(states[0]).max;
    for (unsigned i = 1; i < nstates; ++i)
        max = MAX(max, imm_state_span(states[i]).max);

    return max;
}

static unsigned min_seq(unsigned nstates, struct imm_state **states)
{
    unsigned min = imm_state_span(states[0]).min;
    for (unsigned i = 1; i < nstates; ++i)
        min = MIN(min, imm_state_span(states[i]).min);

    return min;
}

static void set_score(struct imm_dp const *dp, struct imm_task *task,
                      imm_float trans_score, unsigned min_len, unsigned max_len,
                      unsigned row, unsigned state)
{
    for (unsigned len = min_len; len <= max_len; ++len)
    {
        unsigned seq_code = eseq_get(&task->eseq, row, len);
        seq_code -= code_offset(&dp->code, min_len);

        imm_float score = trans_score + emis_score(&dp->emis, state, seq_code);
        matrix_set_score(&task->matrix, row, state, len, score);
    }
}

static void viterbi(struct imm_dp const *dp, struct imm_task *task,
                    struct imm_result *result);
static void viterbi_first_row(struct imm_dp const *dp, struct imm_task *task,
                              unsigned remain);
static void viterbi_first_row_safe(struct imm_dp const *dp,
                                   struct imm_task *task);
static void viterbi_path(struct imm_dp const *dp, struct imm_task const *task,
                         struct imm_path *path, unsigned end_state,
                         unsigned end_seq_len);
static void _viterbi(struct imm_dp const *dp, struct imm_task *task,
                     unsigned const start_row, unsigned const stop_row);
static void _viterbi_safe(struct imm_dp const *dp, struct imm_task *task,
                          unsigned const start_row, unsigned const stop_row);

void imm_dp_init(struct imm_dp *dp, struct imm_abc const *abc)
{
    code_init(&dp->code, abc);
    emis_init(&dp->emis);
    trans_table_init(&dp->trans_table);
    state_table_init(&dp->state_table);
}

void imm_dp_del(struct imm_dp *dp)
{
    if (dp)
    {
        code_del(&dp->code);
        emis_del(&dp->emis);
        trans_table_del(&dp->trans_table);
        state_table_del(&dp->state_table);
    }
}

void dp_reset(struct imm_dp *dp, struct dp_args const *args)
{
    code_reset(&dp->code, min_seq(args->nstates, args->states),
               max_seq(args->nstates, args->states));
    emis_reset(&dp->emis, &dp->code, args->states, args->nstates);
    trans_table_reset(&dp->trans_table, args);
    state_table_reset(&dp->state_table, args);
}

enum imm_rc imm_dp_viterbi(struct imm_dp const *dp, struct imm_task *task,
                           struct imm_result *result)
{
    imm_result_reset(result);
    if (!task->seq)
        return error(IMM_ILLEGALARG, "seq has not been set");

    if (dp->code.abc != imm_seq_abc(task->seq))
        return error(IMM_ILLEGALARG, "dp and seq must have the same alphabet");

    unsigned end_state = dp->state_table.end_state_idx;
    unsigned min = state_table_span(&dp->state_table, end_state).min;
    if (imm_seq_size(task->seq) < min)
        return error(IMM_ILLEGALARG,
                     "seq is shorter than end_state's lower bound");

    struct elapsed elapsed = elapsed_init();
    elapsed_start(&elapsed);
    viterbi(dp, task, result);
    elapsed_end(&elapsed);
    result->seconds = (imm_float)elapsed_seconds(&elapsed);

    return IMM_SUCCESS;
}

unsigned imm_dp_trans_idx(struct imm_dp *dp, unsigned src_idx, unsigned dst_idx)
{
    return trans_table_idx(&dp->trans_table, src_idx, dst_idx);
}

enum imm_rc imm_dp_change_trans(struct imm_dp *dp, unsigned trans_idx,
                                imm_float lprob)
{
    if (imm_unlikely(!imm_lprob_is_finite(lprob)))
        return error(IMM_ILLEGALARG, "lprob must be finite");

    trans_table_change(&dp->trans_table, trans_idx, lprob);
    return IMM_SUCCESS;
}

static struct final_score best_trans_score(struct imm_dp const *dp,
                                           struct matrix const *matrix,
                                           unsigned dst, unsigned row,
                                           uint16_t *best_trans,
                                           uint8_t *best_len)
{
    imm_float score = imm_lprob_zero();
    unsigned prev_state = IMM_STATE_NULL_IDX;
    unsigned prev_seqlen = IMM_STATE_NULL_SEQLEN;
    *best_trans = UINT16_MAX;
    *best_len = UINT8_MAX;

    for (unsigned i = 0; i < trans_table_ntrans(&dp->trans_table, dst); ++i)
    {

        unsigned src = trans_table_source_state(&dp->trans_table, dst, i);
        unsigned min_seq = state_table_span(&dp->state_table, src).min;

        if (imm_unlikely(row < min_seq) || (min_seq == 0 && src > dst))
            continue;

        unsigned max_seq = state_table_span(&dp->state_table, src).max;
        max_seq = (unsigned)MIN(max_seq, row);
        for (unsigned len = min_seq; len <= max_seq; ++len)
        {

            imm_float v0 = matrix_get_score(matrix, row - len, src, len);
            imm_float v1 = trans_table_score(&dp->trans_table, dst, i);
            imm_float v = v0 + v1;

            if (v > score)
            {
                score = v;
                prev_state = src;
                prev_seqlen = len;
                *best_trans = (uint16_t)i;
                *best_len = (uint8_t)(len - min_seq);
            }
        }
    }

    return (struct final_score){score, prev_state, prev_seqlen};
}

static struct final_score
best_trans_score_first_row(struct imm_dp const *dp, struct matrix const *matrix,
                           unsigned dst, uint16_t *best_trans,
                           uint8_t *best_len)
{
    imm_float score = imm_lprob_zero();
    unsigned prev_state = IMM_STATE_NULL_IDX;
    unsigned prev_seq_len = IMM_STATE_NULL_SEQLEN;
    *best_trans = UINT16_MAX;
    *best_len = UINT8_MAX;

    for (unsigned i = 0; i < trans_table_ntrans(&dp->trans_table, dst); ++i)
    {

        unsigned src = trans_table_source_state(&dp->trans_table, dst, i);
        unsigned min_seq = state_table_span(&dp->state_table, src).min;

        if (min_seq > 0 || src > dst)
            continue;

        imm_float v0 = matrix_get_score(matrix, 0, src, 0);
        imm_float v1 = trans_table_score(&dp->trans_table, dst, i);
        imm_float v = v0 + v1;

        if (v > score)
        {
            score = v;
            prev_state = src;
            prev_seq_len = 0;
            *best_trans = (uint16_t)i;
            *best_len = 0;
        }
    }

    return (struct final_score){score, prev_state, prev_seq_len};
}

static struct final_score final_score(struct imm_dp const *dp,
                                      struct imm_task *task)
{
    imm_float score = imm_lprob_zero();
    unsigned end_state = dp->state_table.end_state_idx;

    unsigned final_state = IMM_STATE_NULL_IDX;
    unsigned final_seq_len = IMM_STATE_NULL_SEQLEN;

    unsigned length = eseq_len(&task->eseq);
    unsigned max_seq = state_table_span(&dp->state_table, end_state).max;

    for (unsigned len = (unsigned)MIN(max_seq, length);; --len)
    {

        imm_float s =
            matrix_get_score(&task->matrix, length - len, end_state, len);
        if (s > score)
        {
            score = s;
            final_state = end_state;
            final_seq_len = len;
        }

        if (state_table_span(&dp->state_table, end_state).min == len)
            break;
    }
    struct final_score fscore = {score, final_state, final_seq_len};
    if (final_state == IMM_STATE_NULL_IDX)
        fscore.score = (imm_float)imm_lprob_nan();

    return fscore;
}

static void viterbi(struct imm_dp const *dp, struct imm_task *task,
                    struct imm_result *result)
{
    unsigned len = eseq_len(&task->eseq);

    if (len >= 1 + IMM_STATE_MAX_SEQLEN)
    {
        viterbi_first_row_safe(dp, task);
        _viterbi_safe(dp, task, 1, len - IMM_STATE_MAX_SEQLEN);
        _viterbi(dp, task, len - IMM_STATE_MAX_SEQLEN + 1, len);
    }
    else
    {
        viterbi_first_row(dp, task, len);
        _viterbi(dp, task, 1, len);
    }

    struct final_score const fscore = final_score(dp, task);
    result->loglik = fscore.score;
    viterbi_path(dp, task, &result->path, fscore.state, fscore.seq_len);
}

static void viterbi_first_row(struct imm_dp const *dp, struct imm_task *task,
                              unsigned remain)
{
    for (unsigned i = 0; i < dp->state_table.nstates; ++i)
    {

        uint16_t trans = 0;
        uint8_t len = 0;
        struct final_score tscore =
            best_trans_score_first_row(dp, &task->matrix, i, &trans, &len);
        if (tscore.state != IMM_STATE_NULL_IDX)
        {
            path_set_trans(&task->path, 0, i, trans);
            path_set_seqlen(&task->path, 0, i, len);
            IMM_BUG(path_trans(&task->path, 0, i) != trans);
            IMM_BUG(path_seqlen(&task->path, 0, i) != len);
        }
        else
        {
            path_invalidate(&task->path, 0, i);
            IMM_BUG(path_valid(&task->path, 0, i));
        }

        unsigned min_len = state_table_span(&dp->state_table, i).min;
        unsigned max_len =
            (unsigned)MIN(state_table_span(&dp->state_table, i).max, remain);

        if (dp->state_table.start.state == i)
            tscore.score = MAX(dp->state_table.start.lprob, tscore.score);

        set_score(dp, task, tscore.score, min_len, max_len, 0, i);
    }
}

static void viterbi_first_row_safe(struct imm_dp const *dp,
                                   struct imm_task *task)
{
    for (unsigned i = 0; i < dp->state_table.nstates; ++i)
    {

        uint16_t trans = 0;
        uint8_t len = 0;
        struct final_score tscore =
            best_trans_score_first_row(dp, &task->matrix, i, &trans, &len);
        if (tscore.state != IMM_STATE_NULL_IDX)
        {
            path_set_trans(&task->path, 0, i, trans);
            path_set_seqlen(&task->path, 0, i, len);
            IMM_BUG(path_trans(&task->path, 0, i) != trans);
            IMM_BUG(path_seqlen(&task->path, 0, i) != len);
        }
        else
        {
            path_invalidate(&task->path, 0, i);
            IMM_BUG(path_valid(&task->path, 0, i));
        }

        unsigned min_len = state_table_span(&dp->state_table, i).min;
        unsigned max_len = state_table_span(&dp->state_table, i).max;

        if (dp->state_table.start.state == i)
            tscore.score = MAX(dp->state_table.start.lprob, tscore.score);

        set_score(dp, task, tscore.score, min_len, max_len, 0, i);
    }
}

static void viterbi_path(struct imm_dp const *dp, struct imm_task const *task,
                         struct imm_path *path, unsigned end_state,
                         unsigned end_seq_len)
{
    unsigned row = eseq_len(&task->eseq);
    unsigned state = end_state;
    unsigned seqlen = end_seq_len;
    bool valid = seqlen != IMM_STATE_NULL_SEQLEN;

    while (valid)
    {
        imm_state_id_t id = dp->state_table.ids[state];
        struct imm_step step = imm_step(id, seqlen);
        imm_path_add(path, step);
        row -= seqlen;

        valid = path_valid(&task->path, row, state);
        if (valid)
        {
            unsigned trans = path_trans(&task->path, row, state);
            unsigned len = path_seqlen(&task->path, row, state);
            state = trans_table_source_state(&dp->trans_table, state, trans);
            seqlen = len + state_table_span(&dp->state_table, state).min;
        }
    }
    imm_path_reverse(path);
}

static void _viterbi(struct imm_dp const *dp, struct imm_task *task,
                     unsigned const start_row, unsigned const stop_row)
{
    for (unsigned r = start_row; r <= stop_row; ++r)
    {

        for (unsigned i = 0; i < dp->state_table.nstates; ++i)
        {

            uint16_t trans = 0;
            uint8_t len = 0;
            struct final_score tscore =
                best_trans_score(dp, &task->matrix, i, r, &trans, &len);
            if (tscore.state != IMM_STATE_NULL_IDX)
            {
                path_set_trans(&task->path, r, i, trans);
                path_set_seqlen(&task->path, r, i, len);
                IMM_BUG(path_trans(&task->path, r, i) != trans);
                IMM_BUG(path_seqlen(&task->path, r, i) != len);
            }
            else
            {
                path_invalidate(&task->path, r, i);
                IMM_BUG(path_valid(&task->path, r, i));
            }

            unsigned min_len = state_table_span(&dp->state_table, i).min;
            unsigned max_len = (unsigned)MIN(
                state_table_span(&dp->state_table, i).max, stop_row - r);

            set_score(dp, task, tscore.score, min_len, max_len, r, i);
        }
    }
}

static void _viterbi_safe(struct imm_dp const *dp, struct imm_task *task,
                          unsigned const start_row, unsigned const stop_row)
{
    for (unsigned r = start_row; r <= stop_row; ++r)
    {

        for (unsigned i = 0; i < dp->state_table.nstates; ++i)
        {

            uint16_t trans = 0;
            uint8_t len = 0;
            struct final_score tscore =
                best_trans_score(dp, &task->matrix, i, r, &trans, &len);
            if (tscore.state != IMM_STATE_NULL_IDX)
            {
                path_set_trans(&task->path, r, i, trans);
                path_set_seqlen(&task->path, r, i, len);
                IMM_BUG(path_trans(&task->path, r, i) != trans);
                IMM_BUG(path_seqlen(&task->path, r, i) != len);
            }
            else
            {
                path_invalidate(&task->path, r, i);
                IMM_BUG(path_valid(&task->path, r, i));
            }

            unsigned min_len = state_table_span(&dp->state_table, i).min;
            unsigned max_len = state_table_span(&dp->state_table, i).max;

            set_score(dp, task, tscore.score, min_len, max_len, r, i);
        }
    }
}

#ifndef NDEBUG
void dp_dump_state_table(struct imm_dp const *dp)
{
    state_table_dump(&dp->state_table);
}
#endif
