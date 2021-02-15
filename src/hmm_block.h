#ifndef HMM_BLOCK_H
#define HMM_BLOCK_H

#include "imm/model.h"
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
};

void                        hmm_block_destroy(struct imm_hmm_block const* block);
void                        hmm_block_deep_destroy(struct imm_hmm_block const* block);
struct imm_hmm_block*       hmm_block_new(void);
struct imm_hmm_block const* hmm_block_read(struct imm_model* model, FILE* stream);
int hmm_block_write(struct imm_model const* model, struct imm_hmm_block const* block, FILE* stream);

#endif
