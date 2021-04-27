#ifndef DP_STATE_TABLE_H
#define DP_STATE_TABLE_H

#include "dp_args.h"
#include <stdint.h>
/* #include <stdio.h> */

struct state_table
{
    uint16_t nstates;
    uint8_t *min_seq;
    uint8_t *max_seq;
    struct
    {
        stateidx_t state;
        imm_float lprob;
    } start;
    uint16_t end_state;
};

#define DP_STATE_TABLE_MAX_SEQ 5

struct state_table const *state_table_new(struct dp_args const *args);

void state_table_del(struct state_table const *tbl);

static inline unsigned state_table_max_seq(struct state_table const *tbl,
                                           unsigned state)
{
    return tbl->max_seq[state];
}

static inline unsigned state_table_min_seq(struct state_table const *tbl,
                                           unsigned state)
{
    return tbl->min_seq[state];
}

/* struct state_table *dp_state_table_read(FILE *stream); */

/* int dp_state_table_write(struct state_table const *state_tbl, FILE
 * *stream); */

#endif
