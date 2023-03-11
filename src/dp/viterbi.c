#include "dp/viterbi.h"
#include "dp/assume.h"
#include "dp/best_trans.h"
#include "dp/final_score.h"
#include "dp/minmax.h"
#include "dp/premise.h"
#include "dp/set_score.h"
#include "dp/state_range.h"
#include "dp/state_table.h"
#include "imm/dp.h"
#include "task.h"
#include <assert.h>

void viterbi3(struct premise premise, struct imm_dp const *dp,
              struct imm_task *task, unsigned start_row, unsigned stop_row,
              unsigned seqlen)
{
    if (premise.first_row) ASSUME(start_row == 0 && stop_row == 0);

    for (unsigned r = start_row; r <= stop_row; ++r)
    {
        for (unsigned i = 0; i < dp->state_table.nstates; ++i)
        {
            struct matrix const *matrix = &task->matrix;
            struct final_score2 fs =
                best_trans_score3(premise, dp, matrix, i, r);

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

            if (premise.safe_ceiling) ASSUME(span.max + r <= stop_row);

            if (r == 0)
            {
                // span.min = min_u(span.min, seqlen);
                span.max = min_u(span.max, seqlen);
                if (span.min > span.max) continue;

                if (dp->state_table.start.state == i)
                    fs.score = MAX(dp->state_table.start.lprob, fs.score);

                struct state_range range = state_range(i, span.min, span.max);
                set_multi_score(premise, &dp->emis, task, r, &range, fs.score);
            }
            else
            {
                struct state_range range = state_range(i, span.min, span.max);
                set_multi_score(premise, &dp->emis, task, r, &range, fs.score);
            }

#if 0
            struct span span = state_table_span(&dp->state_table, i);

            if (premise.safe_ceiling) ASSUME(span.max + r <= stop_row);

            if (r == 0)
            {
                if (seqlen < span.min) continue;
                span.max = min_u(span.max, seqlen);
            }
            else
            {
                span.max = min_u(span.max, stop_row - r);
                if (dp->state_table.start.state == i)
                    fs.score = MAX(dp->state_table.start.lprob, fs.score);
            }

            if (span.min > span.max) continue;

            struct state_range range = state_range(i, span.min, span.max);
            set_multi_score(premise, &dp->emis, task, r, &range, fs.score);
#endif
        }
    }
}
