#ifndef DP_STATE_TABLE_H
#define DP_STATE_TABLE_H

#include "dp/span.h"
#include "imm/dp/state_table.h"

struct dp_args;

void state_table_del(struct imm_dp_state_table const *tbl);

void state_table_init(struct imm_dp_state_table *tbl);

enum imm_rc state_table_reset(struct imm_dp_state_table *tbl,
                              struct dp_args const *args);

static inline struct span state_table_span(struct imm_dp_state_table const *tbl,
                                           unsigned state)
{
    return tbl->span[state];
}

unsigned state_table_idx(struct imm_dp_state_table const *tbl,
                         unsigned state_id);

void state_table_dump(struct imm_dp_state_table const *tbl);

#endif
