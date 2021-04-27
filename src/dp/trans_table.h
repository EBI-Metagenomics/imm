#ifndef DP_TRANS_TABLE_H
#define DP_TRANS_TABLE_H

#include "dp_args.h"
#include "imm/state.h"
#include <stdint.h>
/* #include <stdio.h> */

struct imm_hmm;
struct state_idx;

struct trans_table
{
    uint16_t ntrans;  /**< Number of transitions. */
    imm_float *score; /**< Transition score. */
    stateidx_t *src;  /**< Source state. */
    uint16_t *offset; /**< Maps (dest. state, local trans) to score
                        and src. state indices. */
};

int trans_table_change(struct trans_table *trans_tbl, unsigned src,
                       unsigned dst, imm_float lprob);
struct trans_table *trans_table_new(struct dp_args const *args);
void trans_table_destroy(struct trans_table const *transition);
static inline unsigned trans_table_ntrans(struct trans_table const *trans_tbl,
                                          unsigned dst);
struct trans_table *trans_table_read(FILE *stream);
static inline imm_float trans_table_score(struct trans_table const *trans_tbl,
                                          unsigned dst, unsigned trans);
static inline unsigned
trans_table_source_state(struct trans_table const *trans_tbl, unsigned dst,
                         unsigned trans);
static inline unsigned
trans_table_total_ntrans(struct trans_table const *trans_tbl);
int trans_table_write(struct trans_table const *trans, unsigned nstates,
                      FILE *stream);

static inline unsigned trans_table_ntrans(struct trans_table const *trans_tbl,
                                          unsigned dst)
{
    return (unsigned)(trans_tbl->offset[dst + 1] - trans_tbl->offset[dst]);
}

static inline imm_float trans_table_score(struct trans_table const *trans_tbl,
                                          unsigned dst, unsigned trans)
{
    return trans_tbl->score[trans_tbl->offset[dst] + trans];
}

static inline unsigned
trans_table_source_state(struct trans_table const *trans_tbl, unsigned dst,
                         unsigned trans)
{
    return trans_tbl->src[trans_tbl->offset[dst] + trans];
}

static inline unsigned
trans_table_total_ntrans(struct trans_table const *trans_tbl)
{
    return trans_tbl->ntrans;
}

#endif
