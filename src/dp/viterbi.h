#ifndef DP_VITERBI_H
#define DP_VITERBI_H

#include "dp/assume.h"
#include "dp/best_trans.h"
#include "dp/final_score.h"
#include "dp/minmax.h"
#include "dp/set_score.h"
#include "dp/state_range.h"
#include "dp/state_table.h"
#include "imm/dp.h"
#include "task.h"
#include <assert.h>

void viterbi(bool first_row, bool real_past, bool safe_floor, bool safe_ceil,
             enum state_type T, unsigned MINLEN, unsigned MAXLEN,
             struct imm_dp const *dp, struct imm_task *task, unsigned start_row,
             unsigned stop_row)
{
    for (unsigned r = start_row; r <= stop_row; ++r)
    {
        for (unsigned i = 0; i < dp->state_table.nstates; ++i)
        {
            struct final_score tscore =
                best_trans_score(first_row, real_past, safe_floor, T, MINLEN,
                                 MAXLEN, dp, &task->matrix, i, r);

            // I believe this condition can be eliminated if we invalidate
            // the whole path before-hand.
            if (tscore.state != IMM_STATE_NULL_IDX)
            {
                path_set_trans(&task->path, r, i, tscore.trans);
                path_set_seqlen(&task->path, r, i, tscore.seqlen);
                assert(path_trans(&task->path, r, i) == trans);
                assert(path_seqlen(&task->path, r, i) == len);
            }
            else
            {
                path_invalidate(&task->path, r, i);
                assert(!path_valid(&task->path, r, i));
            }

            unsigned min_len = state_table_span(&dp->state_table, i).min;
            unsigned max_len = state_table_span(&dp->state_table, i).max;
            ASSUME_SAFE_CEIL(safe_ceil, max_len + r, stop_row)
            max_len = min_u(max_len, stop_row - r);

            struct state_range range = {i, min_len, max_len};
            set_multi_score(T, MINLEN, MAXLEN, &dp->emis, task, r, &range,
                            tscore.score);
        }
    }
}

#endif
