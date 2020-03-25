#ifndef IMM_DP2_TRANS_H
#define IMM_DP2_TRANS_H

struct mstate;
struct state_idx;

struct dp2_trans
{
    double*   cost;
    unsigned* source_state;
    unsigned* offset;
};

struct dp2_trans const* dp2_trans_create(struct mstate const* const* mstates, unsigned nstates,
                                         struct state_idx* state_idx);

static inline double dp2_trans_ntrans(struct dp2_trans const* trans_tbl, unsigned target_state)
{
    return trans_tbl->offset[target_state + 1] - trans_tbl->offset[target_state];
}

static inline double dp2_trans_cost(struct dp2_trans const* trans_tbl, unsigned target_source,
                                    unsigned trans_idx)
{
    return trans_tbl->cost[trans_tbl->offset[target_source] + trans_idx];
}

static inline unsigned dp2_trans_source_state(struct dp2_trans const* trans_tbl,
                                              unsigned target_state, unsigned trans_idx)
{
    return trans_tbl->source_state[trans_tbl->offset[target_state] + trans_idx];
}

void dp2_trans_destroy(struct dp2_trans const* transition);

#endif
