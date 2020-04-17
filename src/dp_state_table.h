#ifndef DP_STATE_TABLE_H
#define DP_STATE_TABLE_H

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
    double*  start_lprob;
    uint32_t end_state;
};

struct dp_state_table const* dp_state_table_create(struct mstate const* const* mstates,
                                                   uint32_t                    nstates,
                                                   struct imm_state const*     end_state,
                                                   struct state_idx*           state_idx);

static inline uint8_t dp_state_table_min_seq(struct dp_state_table const* state_tbl,
                                             uint32_t                     state)
{
    return state_tbl->min_seq[state];
}

static inline uint8_t dp_state_table_max_seq(struct dp_state_table const* state_tbl,
                                             uint32_t                     state)
{
    return state_tbl->max_seq[state];
}

static inline uint32_t dp_state_table_nstates(struct dp_state_table const* state_tbl)
{
    return state_tbl->nstates;
}

static inline double dp_state_table_start_lprob(struct dp_state_table const* state_tbl,
                                                uint32_t                     state)
{
    return state_tbl->start_lprob[state];
}

static inline uint32_t dp_state_table_end_state(struct dp_state_table const* state_tbl)
{
    return state_tbl->end_state;
}

void dp_state_table_destroy(struct dp_state_table const* state_tbl);

int dp_state_table_write(struct dp_state_table const* state_tbl, FILE* stream);

struct dp_state_table* dp_state_table_read(FILE* stream);

#endif
