#ifndef DP_STATE_TABLE_H
#define DP_STATE_TABLE_H

#include "dp/assume.h"
#include "dp/span.h"
#include "imm/dp/state_table.h"
#include <assert.h>

struct dp_args;

void imm_state_table_del(struct imm_dp_state_table const *tbl);

void imm_state_table_init(struct imm_dp_state_table *tbl);

enum imm_rc imm_state_table_reset(struct imm_dp_state_table *tbl,
                                  struct dp_args const *args);

static inline struct span state_table_span(struct imm_dp_state_table const *tbl,
                                           unsigned state)
{
    struct span span = tbl->span[state];
    assert(span.min <= span.max && !(span.min == 0 && span.max > 0));
    assume(span.min <= span.max && !(span.min == 0 && span.max > 0));
    return span;
}

unsigned imm_state_table_idx(struct imm_dp_state_table const *tbl,
                             unsigned state_id);

static inline unsigned state_table_id(struct imm_dp_state_table const *tbl,
                                      unsigned state_idx)
{
    return tbl->ids[state_idx];
}

void imm_state_table_dump(struct imm_dp_state_table const *tbl);

#endif
