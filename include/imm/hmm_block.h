#ifndef IMM_HMM_BLOCK_H
#define IMM_HMM_BLOCK_H

#include "imm/export.h"
#include <inttypes.h>
#include <stdio.h>

struct imm_abc;
struct imm_dp;
struct imm_hmm;
struct imm_hmm_block;
struct imm_state;

typedef struct imm_state const* (*imm_hmm_block_read_state_cb)(
    struct imm_hmm_block const* hmm_block, struct imm_abc const* abc, FILE* stream, void* args);

typedef int (*imm_hmm_block_write_state_cb)(struct imm_hmm_block const* hmm_block, FILE* stream,
                                            struct imm_state const* state, void* args);

IMM_API struct imm_hmm_block*   imm_hmm_block_create(struct imm_hmm* hmm, struct imm_dp const* dp);
IMM_API void                    imm_hmm_block_destroy(struct imm_hmm_block const* hmm_block);
IMM_API struct imm_dp const*    imm_hmm_block_dp(struct imm_hmm_block const* hmm_block);
IMM_API struct imm_hmm*         imm_hmm_block_hmm(struct imm_hmm_block const* hmm_block);
IMM_API uint32_t                imm_hmm_block_nstates(struct imm_hmm_block const* hmm_block);
IMM_API struct imm_state const* imm_hmm_block_state(struct imm_hmm_block const* hmm_block,
                                                    uint32_t                    i);

IMM_API struct imm_hmm_block* __imm_hmm_block_create(struct imm_hmm* hmm, struct imm_dp const* dp);
IMM_API void                  __imm_hmm_block_deep_destroy(struct imm_hmm_block const* hmm_block);
IMM_API struct imm_hmm_block* __imm_hmm_block_new(void);
IMM_API int __imm_hmm_block_read_dp(struct imm_hmm_block* hmm_block, struct imm_abc const* abc,
                                    FILE* stream);
IMM_API int __imm_hmm_block_read_hmm(struct imm_hmm_block* hmm_block, struct imm_abc const* abc,
                                     imm_hmm_block_read_state_cb read_state, void* read_state_args,
                                     FILE* stream);
IMM_API struct imm_state const* __imm_hmm_block_read_state(struct imm_hmm_block const* hmm_block,
                                                           struct imm_abc const* abc, FILE* stream,
                                                           void* args);
IMM_API int __imm_hmm_block_write_dp(struct imm_hmm_block const* hmm_block, FILE* stream);
IMM_API int __imm_hmm_block_write_hmm(struct imm_hmm_block const* hmm_block, FILE* stream,
                                      imm_hmm_block_write_state_cb write_state,
                                      void*                        write_state_args);
IMM_API int __imm_hmm_block_write_state(struct imm_hmm_block const* hmm_block, FILE* stream,
                                        struct imm_state const* state, void* args);

#endif
