#ifndef DP_SET_SCORE_H
#define DP_SET_SCORE_H

#include "dp/assume.h"
#include "dp/emis.h"
#include "dp/span.h"
#include "dp/state_loc.h"
#include "dp/state_range.h"
#include "imm/dp.h"
#include "task.h"

static inline void set_score(struct imm_dp const *dp, struct imm_task *task,
                             imm_float trans_score, unsigned row,
                             unsigned state, struct span span)
{
    assume(span.max <= IMM_STATE_MAX_SEQLEN);
    for (unsigned i = span.min; i <= span.max; ++i)
    {
        unsigned seq_code = eseq_get(&task->eseq, row, i, span.min);
        imm_float score = trans_score + emis_score(&dp->emis, state, seq_code);
        matrix_set_score(&task->matrix, cell_init(row, state, i), score);
    }
}

#endif
