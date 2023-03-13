#include "dp/viterbi.h"
#include "dp/assume.h"
#include "dp/best_trans.h"
#include "dp/minmax.h"
#include "dp/premise.h"
#include "dp/set_score.h"
#include "dp/state_range.h"
#include "dp/state_table.h"
#include "dp/trans_info.h"
#include "imm/dp.h"
#include "imm/path.h"
#include "imm/prod.h"
#include "task.h"
#include <assert.h>

static imm_float final_score(struct imm_dp const *dp, struct imm_task *task,
                             unsigned *state, unsigned *seqlen);
void viterbi3(struct premise premise, struct imm_dp const *dp,
              struct imm_task *task, unsigned start_row, unsigned stop_row,
              unsigned seqlen);
static enum imm_rc viterbi_path(struct imm_dp const *dp,
                                struct imm_task const *task,
                                struct imm_path *path, unsigned end_state,
                                unsigned end_seq_len);

enum imm_rc viterbi(struct imm_dp const *dp, struct imm_task *task,
                    struct imm_prod *prod)
{
    unsigned len = eseq_len(&task->eseq);
    struct premise premise = {UNKNOWN_STATE, false, false, false, false};

    if (len >= 1 + IMM_STATE_MAX_SEQLEN)
    {
        premise = (struct premise){UNKNOWN_STATE, true, false, false, false};
        viterbi3(premise, dp, task, 0, 0, len);

        premise = (struct premise){UNKNOWN_STATE, false, false, false, true};
        viterbi3(premise, dp, task, 1, len - IMM_STATE_MAX_SEQLEN, len);

        premise = (struct premise){UNKNOWN_STATE, false, false, false, false};
        viterbi3(premise, dp, task, len - IMM_STATE_MAX_SEQLEN + 1, len, len);
    }
    else
    {
        premise = (struct premise){UNKNOWN_STATE, true, false, false, false};
        viterbi3(premise, dp, task, 0, 0, len);

        premise = (struct premise){UNKNOWN_STATE, false, false, false, false};
        viterbi3(premise, dp, task, 1, len, len);
    }

    unsigned state = 0;
    unsigned seqlen = 0;
    prod->loglik = final_score(dp, task, &state, &seqlen);
    return viterbi_path(dp, task, &prod->path, state, seqlen);
}

static enum imm_rc viterbi_path(struct imm_dp const *dp,
                                struct imm_task const *task,
                                struct imm_path *path, unsigned end_state,
                                unsigned end_seq_len)
{
    unsigned row = eseq_len(&task->eseq);
    unsigned state = end_state;
    unsigned seqlen = end_seq_len;
    bool valid = seqlen != IMM_STATE_NULL_SEQLEN;

    while (valid)
    {
        unsigned id = dp->state_table.ids[state];
        struct imm_step step = imm_step(id, seqlen);
        enum imm_rc rc = imm_path_add(path, step);
        if (rc) return rc;
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
    return IMM_OK;
}

static imm_float final_score(struct imm_dp const *dp, struct imm_task *task,
                             unsigned *state, unsigned *seqlen)
{
    imm_float score = imm_lprob_zero();
    unsigned end_state = dp->state_table.end_state_idx;

    *state = IMM_STATE_NULL_IDX;
    *seqlen = IMM_STATE_NULL_SEQLEN;

    unsigned length = eseq_len(&task->eseq);
    unsigned max_seq = state_table_span(&dp->state_table, end_state).max;

    for (unsigned len = (unsigned)MIN(max_seq, length);; --len)
    {

        imm_float s =
            matrix_get_score(&task->matrix, length - len, end_state, len);
        if (s > score)
        {
            score = s;
            *state = end_state;
            *seqlen = len;
        }

        if (state_table_span(&dp->state_table, end_state).min == len) break;
    }

    if (*state == IMM_STATE_NULL_IDX) score = imm_lprob_nan();

    return score;
}

void viterbi3(struct premise premise, struct imm_dp const *dp,
              struct imm_task *task, unsigned start_row, unsigned stop_row,
              unsigned seqlen)
{
    ASSUME(stop_row <= seqlen);
    if (premise.first_row) ASSUME(start_row == 0 && stop_row == 0);

    for (unsigned r = start_row; r <= stop_row; ++r)
    {
        for (unsigned i = 0; i < dp->state_table.nstates; ++i)
        {
            struct matrix const *matrix = &task->matrix;
            struct trans_info fs = best_trans_score3(premise, dp, matrix, i, r);

            // I believe this condition can be eliminated if we invalidate
            // the whole path before-hand.
            if (fs.prev_state != IMM_STATE_NULL_IDX)
            {
                path_set_trans(&task->path, r, i, fs.trans);
                path_set_seqlen(&task->path, r, i, fs.seqlen);
                assert(path_trans(&task->path, r, i) == fs.trans);
                assert(path_seqlen(&task->path, r, i) == fs.seqlen);
            }
            else
            {
                path_invalidate(&task->path, r, i);
                assert(!path_valid(&task->path, r, i));
            }

            struct span span = state_table_span(&dp->state_table, i);
            ASSUME(span.min <= span.max);
            if (seqlen < span.min + r) continue;
            if (seqlen < span.max + r) span.max = seqlen - r;

#if 0
            if (r == 0 && dp->state_table.start.state == i)
                fs.score = MAX(dp->state_table.start.lprob, fs.score);
#endif

            struct state_range range = state_range(i, span.min, span.max);
            set_multi_score(premise, &dp->emis, task, r, &range, fs.score);
        }
    }
}
