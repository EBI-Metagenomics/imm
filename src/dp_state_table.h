#ifndef DP_STATE_TABLE_H
#define DP_STATE_TABLE_H

#include "score.h"
#include "score_code.h"
#include <inttypes.h>
#include <stdio.h>

struct imm_io;
struct imm_state;
struct mstate;
struct state_idx;

struct dp_state_table
{
    uint32_t nstates;
    uint8_t* min_seq;
    uint8_t* max_seq;
    score_t* start_score;
    uint32_t end_state;
};

struct dp_state_table const* dp_state_table_create(struct mstate const* const* mstates,
                                                   uint32_t                    nstates,
                                                   struct imm_state const*     end_state,
                                                   struct state_idx*           state_idx,
                                                   struct score_code const*    score_code);
void                         dp_state_table_destroy(struct dp_state_table const* state_tbl);
static inline uint32_t       dp_state_table_end_state(struct dp_state_table const* state_tbl);
static inline uint8_t        dp_state_table_max_seq(struct dp_state_table const* state_tbl,
                                                    uint32_t                     state);
static inline uint8_t        dp_state_table_min_seq(struct dp_state_table const* state_tbl,
                                                    uint32_t                     state);
static inline uint32_t       dp_state_table_nstates(struct dp_state_table const* state_tbl);
struct dp_state_table*       dp_state_table_read(FILE* stream);
static inline score_t        dp_state_table_start_score(struct dp_state_table const* state_tbl,
                                                        uint32_t                     state);
int dp_state_table_write(struct dp_state_table const* state_tbl, FILE* stream);

static inline uint32_t dp_state_table_end_state(struct dp_state_table const* state_tbl)
{
    return state_tbl->end_state;
}

static inline uint8_t dp_state_table_max_seq(struct dp_state_table const* state_tbl, uint32_t state)
{
    return state_tbl->max_seq[state];
}

static inline uint8_t dp_state_table_min_seq(struct dp_state_table const* state_tbl, uint32_t state)
{
    return state_tbl->min_seq[state];
}

static inline uint32_t dp_state_table_nstates(struct dp_state_table const* state_tbl)
{
    return state_tbl->nstates;
}

static inline score_t dp_state_table_start_score(struct dp_state_table const* state_tbl,
                                                 uint32_t                     state)
{
    return state_tbl->start_score[state];
}

#endif
