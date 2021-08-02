#ifndef DP_TRANS_TABLE_H
#define DP_TRANS_TABLE_H

#include "imm/dp/trans_table.h"

struct dp_args;

unsigned trans_table_idx(struct imm_dp_trans_table *tbl, unsigned src_idx,
                         unsigned dst_idx);

static inline void trans_table_change(struct imm_dp_trans_table *trans_tbl,
                                      unsigned trans_idx, imm_float score)
{
    trans_tbl->trans[trans_idx].score = score;
}

void trans_table_init(struct imm_dp_trans_table *tbl);

void trans_table_reset(struct imm_dp_trans_table *tbl,
                       struct dp_args const *args);

void trans_table_del(struct imm_dp_trans_table const *tbl);

static inline unsigned trans_table_ntrans(struct imm_dp_trans_table const *tbl,
                                          unsigned dst)
{
    return (unsigned)(tbl->offset[dst + 1] - tbl->offset[dst]);
}

static inline imm_float trans_table_score(struct imm_dp_trans_table const *tbl,
                                          unsigned dst, unsigned trans)
{
    return tbl->trans[tbl->offset[dst] + trans].score;
}

static inline unsigned
trans_table_source_state(struct imm_dp_trans_table const *tbl, unsigned dst,
                         unsigned trans)
{
    return tbl->trans[tbl->offset[dst] + trans].src;
}

static inline unsigned trans_table_offsize(unsigned nstates)
{
    return nstates + 1;
}

#endif
