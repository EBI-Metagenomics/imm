#ifndef DP_TRANS_TABLE_H
#define DP_TRANS_TABLE_H

#include "imm/float.h"
#include "imm/state_types.h"

struct dp_args;

struct trans_table
{
    unsigned ntrans; /**< Number of transitions. */
    struct __attribute__((__packed__))
    {
        imm_float score;     /**< Transition score. */
        imm_state_idx_t src; /**< Source state. */
    } * trans;
    uint16_t *offset; /**< Maps (dest. state, local trans) to score
                           and src. state indices. */
};

int trans_table_change(struct trans_table *tbl, unsigned src, unsigned dst,
                       imm_float lprob);

void trans_table_init(struct trans_table *tbl, struct dp_args const *args);

void trans_table_deinit(struct trans_table const *tbl);

static inline unsigned trans_table_ntrans(struct trans_table const *tbl,
                                          unsigned dst)
{
    return tbl->offset[dst + 1] - tbl->offset[dst];
}

static inline imm_float trans_table_score(struct trans_table const *tbl,
                                          unsigned dst, unsigned trans)
{
    return tbl->trans[tbl->offset[dst] + trans].score;
}

static inline unsigned trans_table_source_state(struct trans_table const *tbl,
                                                unsigned dst, unsigned trans)
{
    return tbl->trans[tbl->offset[dst] + trans].src;
}

#endif