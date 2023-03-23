#include "final_score.h"
#include "imm/dp.h"
#include "imm/eseq.h"
#include "imm/lprob.h"
#include "imm/state_table.h"
#include "minmax.h"
#include "span.h"
#include "task.h"

struct final_score final_score(struct imm_dp const *dp, struct imm_task *task)
{
    imm_float score = imm_lprob_zero();
    unsigned end_state = dp->state_table.end_state_idx;

    unsigned final_state = IMM_STATE_NULL_IDX;
    unsigned final_seq_len = IMM_STATE_NULL_SEQLEN;

    unsigned length = imm_eseq_len(&task->eseq);
    unsigned max_seq = imm_state_table_span(&dp->state_table, end_state).max;

    for (unsigned len = min(max_seq, length);; --len)
    {

        imm_float s = imm_matrix_get_score(
            &task->matrix, imm_cell_init(length - len, end_state, len));
        if (s > score)
        {
            score = s;
            final_state = end_state;
            final_seq_len = len;
        }

        if (imm_state_table_span(&dp->state_table, end_state).min == len) break;
    }
    struct final_score fscore = {score, final_state, final_seq_len};
    if (final_state == IMM_STATE_NULL_IDX) fscore.score = imm_lprob_nan();

    return fscore;
}
