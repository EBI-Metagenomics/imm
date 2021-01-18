#ifndef DP_STATE_TABLE_H
#define DP_STATE_TABLE_H

#include "score.h"
#include <inttypes.h>
#include <stdio.h>

struct imm_io;
struct imm_state;
struct model_state;
struct state_idx;

struct dp_state_table
{
    uint16_t nstates;
    uint8_t* min_seq;
    uint8_t* max_seq;
    score_t* start_lprob;
    uint16_t end_state;
};

#define DP_STATE_TABLE_MAX_SEQ 5

struct dp_state_table const* dp_state_table_create(struct model_state const* const* mstates,
                                                   uint_fast16_t                    nstates,
                                                   struct imm_state const*          end_state,
                                                   struct state_idx*                state_idx);
void                         dp_state_table_destroy(struct dp_state_table const* state_tbl);
void                         dp_state_table_dump(struct dp_state_table const* state_tbl);
static inline uint_fast16_t  dp_state_table_end_state(struct dp_state_table const* state_tbl);
static inline uint_fast8_t   dp_state_table_max_seq(struct dp_state_table const* state_tbl,
                                                    uint_fast16_t                state);
static inline uint_fast8_t   dp_state_table_min_seq(struct dp_state_table const* state_tbl,
                                                    uint_fast16_t                state);
static inline uint_fast16_t  dp_state_table_nstates(struct dp_state_table const* state_tbl);
struct dp_state_table*       dp_state_table_read(FILE* stream);
static inline score_t        dp_state_table_start_lprob(struct dp_state_table const* state_tbl,
                                                        uint_fast16_t                state);
int dp_state_table_write(struct dp_state_table const* state_tbl, FILE* stream);

static inline uint_fast16_t dp_state_table_end_state(struct dp_state_table const* state_tbl)
{
    return state_tbl->end_state;
}

static inline uint_fast8_t dp_state_table_max_seq(struct dp_state_table const* state_tbl,
                                                  uint_fast16_t                state)
{
    return state_tbl->max_seq[state];
}

static inline uint_fast8_t dp_state_table_min_seq(struct dp_state_table const* state_tbl,
                                                  uint_fast16_t                state)
{
    return state_tbl->min_seq[state];
}

static inline uint_fast16_t dp_state_table_nstates(struct dp_state_table const* state_tbl)
{
    return state_tbl->nstates;
}

static inline score_t dp_state_table_start_lprob(struct dp_state_table const* state_tbl,
                                                 uint_fast16_t                state)
{
    return state_tbl->start_lprob[state];
}

#endif
