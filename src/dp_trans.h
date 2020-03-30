#ifndef IMM_DP_TRANS_H
#define IMM_DP_TRANS_H

struct mstate;
struct state_idx;

struct dp_trans
{
    double*   score;
    unsigned* source_state;
    unsigned* offset;
};

struct dp_trans const* dp_trans_create(struct mstate const* const* mstates, unsigned nstates,
                                       struct state_idx* state_idx);

static inline double dp_trans_ntrans(struct dp_trans const* trans_tbl, unsigned target_state)
{
    return trans_tbl->offset[target_state + 1] - trans_tbl->offset[target_state];
}

static inline double dp_trans_score(struct dp_trans const* trans_tbl, unsigned target_source,
                                    unsigned trans_idx)
{
    return trans_tbl->score[trans_tbl->offset[target_source] + trans_idx];
}

static inline unsigned dp_trans_source_state(struct dp_trans const* trans_tbl,
                                             unsigned target_state, unsigned trans_idx)
{
    return trans_tbl->source_state[trans_tbl->offset[target_state] + trans_idx];
}

void dp_trans_destroy(struct dp_trans const* transition);

#endif
