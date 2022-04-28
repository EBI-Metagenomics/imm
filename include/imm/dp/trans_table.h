#ifndef IMM_DP_TRANS_TABLE_H
#define IMM_DP_TRANS_TABLE_H

#include "imm/float.h"
#include "imm/state_types.h"
#include "imm/trans.h"

struct imm_dp_trans_table
{
    unsigned ntrans; /**< Number of transitions. */
    struct
    {
        imm_float score;     /**< Transition score. */
        imm_state_idx_t src; /**< Source state. */
    } * trans;
    imm_trans_idx_t *offset; /**< Maps (dest. state, local trans) to score
                                  and src. state indices. */
};

#endif
