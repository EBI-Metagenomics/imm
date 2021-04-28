#include "dp/dp.h"
#include "common/common.h"
#include "dp/args.h"
#include "dp/code.h"
#include "dp/emission.h"
#include "dp/imm_dp.h"
#include "dp/matrix.h"
#include "dp/state_table.h"
#include "dp/task.h"
#include "dp/trans_table.h"
#include "elapsed/elapsed.h"
#include "hmm.h"
#include "imm/error.h"
#include "imm/imm.h"
#include "profile.h"
#include "result.h"
#include <stdint.h>

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
                                      struct imm_dp_task *task);

static unsigned max_seq(unsigned nstates, struct imm_state **states)
{
    unsigned max = imm_state_max_seqlen(states[0]);
    for (unsigned i = 1; i < nstates; ++i)
        max = (unsigned)MAX(max, imm_state_max_seqlen(states[i]));

    return max;
}

static unsigned min_seq(unsigned nstates, struct imm_state **states)
{
    unsigned min = imm_state_min_seqlen(states[0]);
    for (unsigned i = 1; i < nstates; ++i)
        min = (unsigned)MIN(min, imm_state_min_seqlen(states[i]));

    return min;
}

static void set_score(struct imm_dp const *dp, struct imm_dp_task *task,
                      imm_float trans_score, unsigned min_len, unsigned max_len,
                      unsigned row, unsigned state)
{
    for (unsigned len = min_len; len <= max_len; ++len)
    {

        unsigned seq_code = eseq_get(&task->eseq, row, len);
        seq_code -= code_offset(&dp->code, min_len);

        imm_float score =
            trans_score + emission_score(&dp->emission, state, seq_code);
        matrix_set_score(&task->matrix, row, state, len, score);
    }
}

static void viterbi(struct imm_dp const *dp, struct imm_dp_task *task,
                    struct imm_path *path);
static void viterbi_first_row(struct imm_dp const *dp, struct imm_dp_task *task,
                              unsigned remain);
static void viterbi_first_row_safe(struct imm_dp const *dp,
                                   struct imm_dp_task *task);
static void viterbi_path(struct imm_dp const *dp,
                         struct imm_dp_task const *task, struct imm_path *path,
                         unsigned end_state, unsigned end_seq_len);
static void _viterbi(struct imm_dp const *dp, struct imm_dp_task *task,
                     unsigned const start_row, unsigned const stop_row);
static void _viterbi_safe(struct imm_dp const *dp, struct imm_dp_task *task,
                          unsigned const start_row, unsigned const stop_row);

void imm_dp_del(struct imm_dp const *dp)
{
    code_deinit(&dp->code);
    emission_deinit(&dp->emission);
    trans_table_deinit(&dp->trans_table);
    state_table_deinit(&dp->state_table);
    free((void *)dp);
}

struct imm_result const *imm_dp_viterbi(struct imm_dp const *dp,
                                        struct imm_dp_task *task)
{
    if (dp->code.abc != imm_seq_abc(task->seq))
    {
        xerror(IMM_ILLEGALARG, "dp and seq must have the same alphabet");
        return NULL;
    }

    unsigned end_state = dp->state_table.end_state;
    unsigned min = state_table_min_seqlen(&dp->state_table, end_state);
    if (imm_seq_len(task->seq) < min)
    {
        xerror(IMM_ILLEGALARG, "seq is shorter than end_state's lower bound");
        return NULL;
    }

    struct imm_result *result = imm_result_create(task->seq);

    struct imm_path *path = imm_path_new();
    struct elapsed elapsed = elapsed_init();
    elapsed_start(&elapsed);
    viterbi(dp, task, path);
    elapsed_end(&elapsed);
    result_set(result, path, (imm_float)elapsed_seconds(&elapsed));

    return result;
}

int imm_dp_change_trans(struct imm_dp *dp, imm_state_idx_t src,
                        imm_state_idx_t dst, imm_float lprob)
{
    if (!imm_lprob_is_valid(lprob))
    {
        error("invalid lprob");
        return IMM_ILLEGALARG;
    }

    return trans_table_change(&dp->trans_table, src, dst, lprob);
}

struct imm_dp *dp_new(struct dp_args const *args)
{
    struct imm_dp *dp = xmalloc(sizeof(*dp));

    code_init(&dp->code, args->end_state->abc,
              min_seq(args->nstates, args->states),
              max_seq(args->nstates, args->states));

    state_table_init(&dp->state_table, args);
    emission_init(&dp->emission, &dp->code, args->states, args->nstates);
    trans_table_init(&dp->trans_table, args);

    return dp;
}

#if 0
struct imm_dp *dp_create_from(struct imm_state **states,
                              struct code const *seq_code,
                              struct dp_emission const *emission,
                              struct dp_trans_table *trans_table,
                              struct dp_state_table const *state_table)
{
    struct imm_dp *dp = xmalloc(sizeof(*dp));

    dp->states = states;
    dp->seq_code = seq_code;
    dp->emission = emission;
    dp->trans_table = trans_table;
    dp->state_table = state_table;

    dp->state_idx = state_idx_create(state_table_nstates(dp->state_table));
    for (unsigned i = 0; i < state_table_nstates(dp->state_table); ++i)
        state_idx_add(dp->state_idx, dp->states[i]);

    return dp;
}

struct dp_emission const *dp_get_emission(struct imm_dp const *dp)
{
    return dp->emission;
}

struct imm_state **dp_get_states(struct imm_dp const *dp) { return dp->states; }

struct code const *dp_get_seq_code(struct imm_dp const *dp)
{
    return dp->seq_code;
}

struct dp_state_table const *dp_get_state_table(struct imm_dp const *dp)
{
    return dp->state_table;
}

struct dp_trans_table *dp_get_trans_table(struct imm_dp const *dp)
{
    return dp->trans_table;
}
#endif

static struct final_score best_trans_score(struct imm_dp const *dp,
                                           struct matrix const *matrix,
                                           unsigned dst, unsigned row,
                                           uint16_t *best_trans,
                                           uint8_t *best_len)
{
    imm_float score = imm_lprob_zero();
    unsigned prev_state = INVALID_STATE;
    unsigned prev_seqlen = INVALID_SEQLEN;
    *best_trans = UINT16_MAX;
    *best_len = UINT8_MAX;

    for (unsigned i = 0; i < trans_table_ntrans(&dp->trans_table, dst); ++i)
    {

        unsigned src = trans_table_source_state(&dp->trans_table, dst, i);
        unsigned min_seq = state_table_min_seqlen(&dp->state_table, src);

        if (unlikely(row < min_seq) || (min_seq == 0 && src > dst))
            continue;

        unsigned max_seq = state_table_max_seqlen(&dp->state_table, src);
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
    unsigned prev_state = INVALID_STATE;
    unsigned prev_seq_len = INVALID_SEQLEN;
    *best_trans = UINT16_MAX;
    *best_len = UINT8_MAX;

    for (unsigned i = 0; i < trans_table_ntrans(&dp->trans_table, dst); ++i)
    {

        unsigned src = trans_table_source_state(&dp->trans_table, dst, i);
        unsigned min_seq = state_table_min_seqlen(&dp->state_table, src);

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
                                      struct imm_dp_task *task)
{
    imm_float score = imm_lprob_zero();
    unsigned end_state = dp->state_table.end_state;

    unsigned final_state = INVALID_STATE;
    unsigned final_seq_len = INVALID_SEQLEN;

    unsigned length = eseq_len(&task->eseq);
    unsigned max_seq = state_table_max_seqlen(&dp->state_table, end_state);

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

        if (state_table_min_seqlen(&dp->state_table, end_state) == len)
            break;
    }
    struct final_score fscore = {score, final_state, final_seq_len};
    if (final_state == INVALID_STATE)
        fscore.score = (imm_float)imm_lprob_invalid();

    return fscore;
}

static void viterbi(struct imm_dp const *dp, struct imm_dp_task *task,
                    struct imm_path *path)
{
    unsigned const len = eseq_len(&task->eseq);

    if (len >= 1 + DP_STATE_TABLE_MAX_SEQ)
    {
        viterbi_first_row_safe(dp, task);
        _viterbi_safe(dp, task, 1, len - DP_STATE_TABLE_MAX_SEQ);
        _viterbi(dp, task, len - DP_STATE_TABLE_MAX_SEQ + 1, len);
    }
    else
    {
        viterbi_first_row(dp, task, len);
        _viterbi(dp, task, 1, len);
    }

    struct final_score const fscore = final_score(dp, task);
    viterbi_path(dp, task, path, fscore.state, fscore.seq_len);
}

static void viterbi_first_row(struct imm_dp const *dp, struct imm_dp_task *task,
                              unsigned remain)
{
    for (unsigned i = 0; i < dp->state_table.nstates; ++i)
    {

        uint16_t trans = 0;
        uint8_t len = 0;
        struct final_score tscore =
            best_trans_score_first_row(dp, &task->matrix, i, &trans, &len);
        if (tscore.state != INVALID_STATE)
        {
            path_set_trans(&task->path, 0, i, trans);
            path_set_seqlen(&task->path, 0, i, len);
            BUG(path_trans(&task->path, 0, i) != trans);
            BUG(path_seqlen(&task->path, 0, i) != len);
        }
        else
        {
            path_invalidate(&task->path, 0, i);
            BUG(path_valid(&task->path, 0, i));
        }

        unsigned min_len = state_table_min_seqlen(&dp->state_table, i);
        unsigned max_len =
            (unsigned)MIN(state_table_max_seqlen(&dp->state_table, i), remain);

        if (dp->state_table.start.state == i)
            tscore.score = MAX(dp->state_table.start.lprob, tscore.score);

        set_score(dp, task, tscore.score, min_len, max_len, 0, i);
    }
}

static void viterbi_first_row_safe(struct imm_dp const *dp,
                                   struct imm_dp_task *task)
{
    for (unsigned i = 0; i < dp->state_table.nstates; ++i)
    {

        uint16_t trans = 0;
        uint8_t len = 0;
        struct final_score tscore =
            best_trans_score_first_row(dp, &task->matrix, i, &trans, &len);
        if (tscore.state != INVALID_STATE)
        {
            path_set_trans(&task->path, 0, i, trans);
            path_set_seqlen(&task->path, 0, i, len);
            BUG(path_trans(&task->path, 0, i) != trans);
            BUG(path_seqlen(&task->path, 0, i) != len);
        }
        else
        {
            path_invalidate(&task->path, 0, i);
            BUG(path_valid(&task->path, 0, i));
        }

        unsigned min_len = state_table_min_seqlen(&dp->state_table, i);
        unsigned max_len = state_table_max_seqlen(&dp->state_table, i);

        if (dp->state_table.start.state == i)
            tscore.score = MAX(dp->state_table.start.lprob, tscore.score);

        set_score(dp, task, tscore.score, min_len, max_len, 0, i);
    }
}

static void viterbi_path(struct imm_dp const *dp,
                         struct imm_dp_task const *task, struct imm_path *path,
                         unsigned end_state, unsigned end_seq_len)
{
    /* unsigned row = eseq_len(&task->eseq); */
    /* unsigned state = end_state; */
    unsigned seq_len = end_seq_len;
    bool valid = seq_len != INVALID_SEQLEN;

    while (valid)
    {
#if 0
        struct imm_state const *s = dp->states[state];
        struct imm_step *new_step = imm_step_create(s, (uint8_t)seq_len);
        imm_path_prepend(path, new_step);
        row -= seq_len;

        valid = cpath_valid(&task->cpath, row, state);
        if (valid)
        {
            unsigned trans =
                (unsigned)cpath_get_trans(&task->cpath, row, state);
            unsigned len = (unsigned)cpath_get_len(&task->cpath, row, state);
            state = dp_trans_table_source_state(dp->trans_table, state, trans);
            seq_len = (unsigned)(len + dp_state_table_min_seq(dp->state_table,
                                                              state));
        }
#endif
    }
}

static void _viterbi(struct imm_dp const *dp, struct imm_dp_task *task,
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
            if (tscore.state != INVALID_STATE)
            {
                path_set_trans(&task->path, r, i, trans);
                path_set_seqlen(&task->path, r, i, len);
                BUG(path_trans(&task->path, r, i) != trans);
                BUG(path_seqlen(&task->path, r, i) != len);
            }
            else
            {
                path_invalidate(&task->path, r, i);
                BUG(path_valid(&task->path, r, i));
            }

            unsigned min_len = state_table_min_seqlen(&dp->state_table, i);
            unsigned max_len = (unsigned)MIN(
                state_table_max_seqlen(&dp->state_table, i), stop_row - r);

            set_score(dp, task, tscore.score, min_len, max_len, r, i);
        }
    }
}

static void _viterbi_safe(struct imm_dp const *dp, struct imm_dp_task *task,
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
            if (tscore.state != INVALID_STATE)
            {
                path_set_trans(&task->path, r, i, trans);
                path_set_seqlen(&task->path, r, i, len);
                BUG(path_trans(&task->path, r, i) != trans);
                BUG(path_seqlen(&task->path, r, i) != len);
            }
            else
            {
                path_invalidate(&task->path, r, i);
                BUG(path_valid(&task->path, r, i));
            }

            unsigned min_len = state_table_min_seqlen(&dp->state_table, i);
            unsigned max_len = state_table_max_seqlen(&dp->state_table, i);

            set_score(dp, task, tscore.score, min_len, max_len, r, i);
        }
    }
}
