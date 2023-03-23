#ifndef IMM_DP_TRANS_TABLE_H
#define IMM_DP_TRANS_TABLE_H

#include "imm/float.h"
#include "imm/state_types.h"
#include "imm/trans.h"

struct imm_trans_table
{
    unsigned ntrans; /**< Number of transitions. */
    struct
    {
        imm_float score;     /**< Transition score. */
        imm_state_idx_t src; /**< Source state. */
    } *trans;
    imm_trans_idx_t *offset; /**< Maps (dest. state, local trans) to score
                                  and src. state indices. */
};

struct imm_dp_args;

void imm_trans_table_init(struct imm_trans_table *);
int imm_trans_table_reset(struct imm_trans_table *, struct imm_dp_args const *);

unsigned imm_trans_table_idx(struct imm_trans_table const *, unsigned src,
                             unsigned dst);

void imm_trans_table_change(struct imm_trans_table *, unsigned trans,
                            imm_float score);

void imm_trans_table_del(struct imm_trans_table const *);

unsigned imm_trans_table_ntrans(struct imm_trans_table const *, unsigned dst);

imm_float imm_trans_table_score(struct imm_trans_table const *, unsigned dst,
                                unsigned trans);

unsigned imm_trans_table_source_state(struct imm_trans_table const *,
                                      unsigned dst, unsigned trans);

unsigned imm_trans_table_offsize(unsigned nstates);

#endif
