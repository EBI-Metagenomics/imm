#include "hmm_block.h"
#include "dp.h"
#include "dp_emission.h"
#include "dp_state_table.h"
#include "dp_trans_table.h"
#include "free.h"
#include "hmm.h"
#include "imm/dp.h"
#include "imm/hmm.h"
#include "imm/hmm_block.h"
#include "model_state.h"
#include "seq_code.h"
#include <stdlib.h>

struct imm_dp const* imm_hmm_block_dp(struct imm_hmm_block const* block) { return block->dp; }

struct imm_hmm* imm_hmm_block_hmm(struct imm_hmm_block const* block) { return block->hmm; }

uint16_t imm_hmm_block_nstates(struct imm_hmm_block const* block) { return block->nstates; }

struct imm_state const* imm_hmm_block_state(struct imm_hmm_block const* block, uint16_t i)
{
    return block->states[i];
}

struct imm_hmm_block* hmm_block_new(void)
{
    struct imm_hmm_block* block = malloc(sizeof(*block));
    block->hmm = NULL;
    block->mstates = NULL;
    block->nstates = 0;
    block->states = NULL;
    block->seq_code = NULL;
    block->emission = NULL;
    block->trans_table = NULL;
    block->state_table = NULL;
    block->dp = NULL;
    return block;
}

struct imm_hmm_block* hmm_block_create(struct imm_hmm* hmm, struct imm_dp const* dp)
{
    struct imm_hmm_block* block = hmm_block_new();
    block->hmm = hmm;
    block->mstates = (struct model_state**)hmm_get_mstates(hmm, dp);

    block->nstates = (uint16_t)dp_state_table_nstates(dp_get_state_table(dp));
    block->states = malloc(sizeof(*block->states) * block->nstates);
    for (uint16_t i = 0; i < block->nstates; ++i)
        block->states[i] = block->mstates[i]->state;

    block->seq_code = dp_get_seq_code(dp);
    block->emission = dp_get_emission(dp);
    block->trans_table = dp_get_trans_table(dp);
    block->state_table = dp_get_state_table(dp);
    block->dp = dp;
    return block;
}

void hmm_block_destroy(struct imm_hmm_block const* block)
{
    free_c(block->states);
    free_c(block);
}

void hmm_block_deep_destroy(struct imm_hmm_block const* block)
{
    if (block->hmm)
        imm_hmm_destroy(block->hmm);

    if (block->mstates)
        free_c(block->mstates);

    if (block->states)
        free_c(block->states);

    if (block->seq_code)
        seq_code_destroy(block->seq_code);

    if (block->emission)
        dp_emission_destroy(block->emission);

    if (block->trans_table)
        dp_trans_table_destroy(block->trans_table);

    if (block->state_table)
        dp_state_table_destroy(block->state_table);

    if (block->dp)
        imm_dp_destroy(block->dp);
}
