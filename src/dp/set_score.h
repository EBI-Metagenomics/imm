#ifndef DP_SET_SCORE_H
#define DP_SET_SCORE_H

#include "dp/assume.h"
#include "dp/emis.h"
#include "dp/state_loc.h"
#include "dp/state_range.h"
#include "imm/dp.h"
#include "task.h"

#if 0
static inline void set_single_score(struct imm_dp_emis const *x,
                                    struct imm_task *t, unsigned row,
                                    struct state_loc const *loc,
                                    imm_float tscore)
{
    unsigned seq_code = eseq_get(&t->eseq, row, loc->len, loc->min_len);
    imm_float score = tscore + emis_score(x, loc->state, seq_code);
    matrix_set_score(&t->matrix, row, loc->state, loc->len, score);
}

static inline void set_multi_score(struct imm_dp_emis const *x,
                                   struct imm_task *t, unsigned row,
                                   struct state_range const *range,
                                   imm_float tscore)
{
    for (unsigned len = range->min_len; len <= range->max_len; ++len)
    {
        struct state_loc loc = {range->state, range->min_len, len};
        set_single_score(x, t, row, &loc, tscore);
    }
}
#endif

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

#endif
