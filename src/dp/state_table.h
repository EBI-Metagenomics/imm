#ifndef DP_STATE_TABLE_H
#define DP_STATE_TABLE_H

#include "dp/span.h"
#include "imm/state.h"

struct dp_args;

struct state_table
{
    unsigned nstates;
    imm_state_id_t *ids;
    struct
    {
        imm_state_idx_t state;
        imm_float lprob;
    } start;
    unsigned end_state;
    struct span *span;
};

#define STATE_TABLE_MAX_SEQ 5

void state_table_del(struct state_table const *tbl);

void state_table_init(struct state_table *tbl);

void state_table_reset(struct state_table *tbl, struct dp_args const *args);

static inline struct span state_table_span(struct state_table const *tbl,
                                           unsigned state)
{
    return tbl->span[state];
}

#ifndef NDEBUG
void state_table_dump(struct state_table const *tbl);
#endif

#endif
