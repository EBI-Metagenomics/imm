#ifndef DP_FINAL_SCORE_H
#define DP_FINAL_SCORE_H

#include "dp/minmax.h"
#include "dp/state_table.h"
#include "imm/dp.h"
#include "imm/float.h"
#include "imm/lprob.h"
#include "task.h"

struct final_score
{
    imm_float score;
    unsigned state;
    unsigned seq_len;
};

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

        if (state_table_span(&dp->state_table, end_state).min == len) break;
    }
    struct final_score fscore = {score, final_state, final_seq_len};
    if (final_state == IMM_STATE_NULL_IDX)
        fscore.score = (imm_float)imm_lprob_nan();

    return fscore;
}

#endif
