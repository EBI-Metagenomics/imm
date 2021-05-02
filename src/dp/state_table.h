#ifndef DP_STATE_TABLE_H
#define DP_STATE_TABLE_H

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
    union
    {
        unsigned char bytes[2];
        struct __attribute__((__packed__))
        {
            uint8_t min;
            uint8_t max;
        };
    } * seqlen;
};

#define STATE_TABLE_MAX_SEQ 5

void state_table_deinit(struct state_table const *tbl);

void state_table_init(struct state_table *tbl, struct dp_args const *args);

static inline unsigned state_table_max_seqlen(struct state_table const *tbl,
                                              unsigned state)
{
    return tbl->seqlen[state].max;
}

static inline unsigned state_table_min_seqlen(struct state_table const *tbl,
                                              unsigned state)
{
    return tbl->seqlen[state].min;
}

#ifndef NDEBUG
void state_table_dump(struct state_table const *tbl);
#endif

#endif
