#ifndef HMM_BLOCK_H
#define HMM_BLOCK_H

#include "imm/model.h"
#include "list.h"
#include <inttypes.h>

struct dp_emission;
struct dp_state_table;
struct imm_hmm;
struct model_state;
struct seq_code;

struct imm_hmm_block
{
    struct imm_hmm*              hmm;
    struct model_state**         mstates;
    uint16_t                     nstates;
    struct imm_state const**     states;
    struct seq_code const*       seq_code;
    struct dp_emission const*    emission;
    struct dp_trans_table*       trans_table;
    struct dp_state_table const* state_table;
    struct imm_dp const*         dp;

    struct list_head list_entry;
};

struct imm_hmm_block* hmm_block_create(struct imm_hmm* hmm, struct imm_dp const* dp);
void                  hmm_block_destroy(struct imm_hmm_block const* block);
void                  hmm_block_deep_destroy(struct imm_hmm_block const* block);
struct imm_hmm_block* hmm_block_new(void);

#endif
