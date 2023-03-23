#ifndef DP_H
#define DP_H

#if 0
#include "imm/dp.h"
#include "imm/float.h"
#include "imm/state_table.h"
#include "span.h"
#include "trans_table.h"

struct imm_dp;
struct imm_state;

enum imm_rc imm_dp_reset(struct imm_dp *dp, struct dp_args const *args);

static inline unsigned dp_start_state(struct imm_dp const *x)
{
    return x->state_table.start.state;
}

static inline imm_float dp_start_lprob(struct imm_dp const *x)
{
    return x->state_table.start.lprob;
}

static inline struct span dp_state_span(struct imm_dp const *x, unsigned i)
{
    return imm_state_table_span(&x->state_table, i);
}

static inline imm_float dp_trans_score(struct imm_dp const *x, unsigned dst,
                                       unsigned trans)
{
    return trans_table_score(&x->trans_table, dst, trans);
}
#endif

#endif
