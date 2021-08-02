#ifndef IMM_DP_STATE_TABLE_H
#define IMM_DP_STATE_TABLE_H

#include "imm/state_types.h"

struct span;

struct imm_dp_state_table
{
    unsigned nstates;
    imm_state_id_t *ids;
    struct
    {
        imm_state_idx_t state;
        imm_float lprob;
    } start;
    unsigned end_state_idx;
    struct span *span;
};

#endif
