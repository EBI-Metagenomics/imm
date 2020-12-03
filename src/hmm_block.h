#ifndef HMM_BLOCK_H
#define HMM_BLOCK_H

#include <inttypes.h>

struct dp_emission;
struct dp_state_table;
struct dp_trans_table;
struct imm_abc;
struct imm_dp;
struct imm_hmm;
struct imm_state;
struct mstate;
struct seq_code;

struct imm_hmm_block
{
    struct imm_hmm*              hmm;
    struct mstate**              mstates;
    uint32_t                     nstates;
    struct imm_state const**     states;
    struct seq_code const*       seq_code;
    struct dp_emission const*    emission;
    struct dp_trans_table*       trans_table;
    struct dp_state_table const* state_table;
    struct imm_dp const*         dp;
};

#endif
