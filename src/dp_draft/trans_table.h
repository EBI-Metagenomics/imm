#ifndef IMM_DP_TRANS_TABLE_H
#define IMM_DP_TRANS_TABLE_H

struct mstate;
struct state_idx;

struct dp_trans_table
{
    double*   cost;
    unsigned* target_state;
    unsigned* offset;
};

struct dp_trans_table const* dp_trans_table_create(struct mstate const* const* mstates,
                                                   unsigned                    nstates,
                                                   struct state_idx*           state_idx);

static inline double dp_trans_table_ntrans(struct dp_trans_table const* trans_tbl,
                                           unsigned                     source_state)
{
    return trans_tbl->offset[source_state + 1] - trans_tbl->offset[source_state];
}

static inline double dp_trans_table_cost(struct dp_trans_table const* trans_tbl,
                                         unsigned source_state, unsigned trans_idx)
{
    return trans_tbl->cost[trans_tbl->offset[source_state] + trans_idx];
}

static inline unsigned dp_trans_table_target_state(struct dp_trans_table const* trans_tbl,
                                                   unsigned source_state, unsigned trans_idx)
{
    return trans_tbl->target_state[trans_tbl->offset[source_state] + trans_idx];
}

void dp_trans_table_destroy(struct dp_trans_table const* transition);

#endif
