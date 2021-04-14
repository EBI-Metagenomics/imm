#ifndef DP_TRANS_TABLE_H
#define DP_TRANS_TABLE_H

#include "imm/imm.h"
#include <inttypes.h>
#include <stdio.h>

struct imm_io;
struct model_state;
struct state_idx;

struct dp_trans_table
{
    uint16_t   ntrans;       /**< Number of transitions. */
    imm_float* score;        /**< Transition score. */
    uint16_t*  source_state; /**< Source state. */
    uint16_t*  offset;       /**< Maps (target state, local trans) to score
                               and source_state indices. */
};

int dp_trans_table_change(struct dp_trans_table* trans_tbl, uint_fast16_t src_state, uint_fast16_t tgt_state,
                          imm_float lprob);
struct dp_trans_table*      dp_trans_table_create(struct model_state const* const* mstates, uint_fast16_t nstates,
                                                  struct state_idx* state_idx);
void                        dp_trans_table_destroy(struct dp_trans_table const* transition);
static inline uint_fast16_t dp_trans_table_ntrans(struct dp_trans_table const* trans_tbl, uint_fast16_t tgt_state);
struct dp_trans_table*      dp_trans_table_read(FILE* stream);
static inline imm_float     dp_trans_table_score(struct dp_trans_table const* trans_tbl, uint_fast16_t tgt_state,
                                                 uint_fast16_t trans);
static inline uint_fast16_t dp_trans_table_source_state(struct dp_trans_table const* trans_tbl, uint_fast16_t tgt_state,
                                                        uint_fast16_t trans);
static inline uint_fast16_t dp_trans_table_total_ntrans(struct dp_trans_table const* trans_tbl);
int dp_trans_table_write(struct dp_trans_table const* trans, uint_fast16_t nstates, FILE* stream);

static inline uint_fast16_t dp_trans_table_ntrans(struct dp_trans_table const* trans_tbl, uint_fast16_t tgt_state)
{
    return (uint_fast16_t)(trans_tbl->offset[tgt_state + 1] - trans_tbl->offset[tgt_state]);
}

static inline imm_float dp_trans_table_score(struct dp_trans_table const* trans_tbl, uint_fast16_t tgt_state,
                                             uint_fast16_t trans)
{
    return trans_tbl->score[trans_tbl->offset[tgt_state] + trans];
}

static inline uint_fast16_t dp_trans_table_source_state(struct dp_trans_table const* trans_tbl, uint_fast16_t tgt_state,
                                                        uint_fast16_t trans)
{
    return trans_tbl->source_state[trans_tbl->offset[tgt_state] + trans];
}

static inline uint_fast16_t dp_trans_table_total_ntrans(struct dp_trans_table const* trans_tbl)
{
    return trans_tbl->ntrans;
}

#endif
