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

int dp_trans_table_change(struct dp_trans_table* trans_tbl, uint32_t src_state, uint32_t tgt_state,
                          double lprob);
struct dp_trans_table* dp_trans_table_create(struct mstate const* const* mstates, uint32_t nstates,
                                             struct state_idx* state_idx);
void                   dp_trans_table_destroy(struct dp_trans_table const* transition);
static inline uint32_t dp_trans_table_ntrans(struct dp_trans_table const* trans_tbl,
                                             uint32_t                     tgt_state);
struct dp_trans_table* dp_trans_table_read(FILE* stream);
static inline double   dp_trans_table_score(struct dp_trans_table const* trans_tbl,
                                            uint32_t tgt_state, uint32_t trans);
static inline uint32_t dp_trans_table_source_state(struct dp_trans_table const* trans_tbl,
                                                   uint32_t tgt_state, uint32_t trans);
static inline uint32_t dp_trans_table_total_ntrans(struct dp_trans_table const* trans_tbl);
int dp_trans_table_write(struct dp_trans_table const* trans, uint32_t nstates, FILE* stream);

static inline uint32_t dp_trans_table_ntrans(struct dp_trans_table const* trans_tbl,
                                             uint32_t                     tgt_state)
{
    return trans_tbl->offset[tgt_state + 1] - trans_tbl->offset[tgt_state];
}

static inline double dp_trans_table_score(struct dp_trans_table const* trans_tbl,
                                          uint32_t tgt_state, uint32_t trans)
{
    return trans_tbl->score[trans_tbl->offset[tgt_state] + trans];
}

static inline uint32_t dp_trans_table_source_state(struct dp_trans_table const* trans_tbl,
                                                   uint32_t tgt_state, uint32_t trans)
{
    return trans_tbl->source_state[trans_tbl->offset[tgt_state] + trans];
}

static inline uint32_t dp_trans_table_total_ntrans(struct dp_trans_table const* trans_tbl)
{
    return trans_tbl->ntrans;
}

#endif
