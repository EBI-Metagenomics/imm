#ifndef DP_TRANS_TABLE_H
#define DP_TRANS_TABLE_H

#include <inttypes.h>
#include <stdio.h>

struct imm_io;
struct mstate;
struct state_idx;

struct dp_trans_table
{
    uint32_t  ntrans;
    double*   score;
    uint32_t* source_state;
    uint32_t* offset;
};

struct dp_trans_table const* dp_trans_table_create(struct mstate const* const* mstates,
                                                   uint32_t                    nstates,
                                                   struct state_idx*           state_idx);

static inline uint32_t dp_trans_table_total_ntrans(struct dp_trans_table const* trans_tbl)
{
    return trans_tbl->ntrans;
}

static inline uint32_t dp_trans_table_ntrans(struct dp_trans_table const* trans_tbl,
                                             uint32_t                     target_state)
{
    return trans_tbl->offset[target_state + 1] - trans_tbl->offset[target_state];
}

static inline double dp_trans_table_score(struct dp_trans_table const* trans_tbl,
                                          uint32_t target_state, uint32_t trans)
{
    return trans_tbl->score[trans_tbl->offset[target_state] + trans];
}

static inline uint32_t dp_trans_table_source_state(struct dp_trans_table const* trans_tbl,
                                                   uint32_t target_state, uint32_t trans)
{
    return trans_tbl->source_state[trans_tbl->offset[target_state] + trans];
}

void dp_trans_table_destroy(struct dp_trans_table const* transition);

int dp_trans_table_write(struct dp_trans_table const* trans, uint32_t nstates, FILE* stream);

struct dp_trans_table const* dp_trans_table_read(FILE* stream);

#endif
