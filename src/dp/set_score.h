#ifndef DP_SET_SCORE_H
#define DP_SET_SCORE_H

#include "dp/assume.h"
#include "dp/emis.h"
#include "dp/matrix.h"
#include "dp/state_loc.h"
#include "dp/state_range.h"
#include "eseq.h"
#include "imm/dp.h"
#include "imm/dp/emis.h"
#include "imm/float.h"
#include "task.h"

static inline void set_single_score(struct imm_dp_emis const *x,
                                    struct imm_task *t, unsigned row,
                                    struct state_loc const *loc,
                                    imm_float tscore)
{
    unsigned seq_code = eseq_get(&t->eseq, row, loc->len, loc->min_len);
    imm_float score = tscore + emis_score(x, loc->state, seq_code);
    matrix_set_score(&t->matrix, row, loc->state, loc->len, score);
}

static inline void set_multi_score(enum state_type T, unsigned MINLEN,
                                   unsigned MAXLEN, struct imm_dp_emis const *x,
                                   struct imm_task *t, unsigned row,
                                   struct state_range const *range,
                                   imm_float tscore)
{
    ASSUME_MIXLEN(T, MINLEN, MAXLEN, range->min_len, range->max_len);
    for (unsigned len = range->min_len; len <= range->max_len; ++len)
    {
        struct state_loc loc = {range->state, range->min_len, len};
        set_single_score(x, t, row, &loc, tscore);
    }
}

#endif
