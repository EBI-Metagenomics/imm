#ifndef IMM_HMM_BLOCK_H
#define IMM_HMM_BLOCK_H

#include "imm/export.h"
#include <inttypes.h>
#include <stdio.h>

struct imm_dp;
struct imm_hmm;
struct imm_hmm_block;
struct imm_model;

IMM_API struct imm_dp const*    imm_hmm_block_dp(struct imm_hmm_block const* block);
IMM_API struct imm_hmm*         imm_hmm_block_hmm(struct imm_hmm_block const* block);
IMM_API uint16_t                imm_hmm_block_nstates(struct imm_hmm_block const* block);
IMM_API struct imm_state const* imm_hmm_block_state(struct imm_hmm_block const* block, uint16_t i);

#endif
