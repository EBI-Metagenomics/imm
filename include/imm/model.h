#ifndef IMM_MODEL_H
#define IMM_MODEL_H

#include "imm/export.h"
#include <inttypes.h>
#include <stdio.h>

struct imm_abc;
struct imm_dp;
struct imm_hmm;
struct imm_model;
struct imm_hmm_block;

typedef struct imm_state const* (*imm_model_read_state_cb)(struct imm_model const* model,
                                                           FILE* stream, void* args);
typedef int (*imm_model_write_state_cb)(struct imm_model const* model, FILE* stream,
                                        struct imm_state const* state, void* args);

IMM_API struct imm_abc const* imm_model_abc(struct imm_model const* model);
IMM_API struct imm_model*     imm_model_create(struct imm_hmm* hmm, struct imm_dp const* dp);
IMM_API void imm_model_append_hmm_block(struct imm_model* model, struct imm_hmm* hmm,
                                        struct imm_dp const* dp);
IMM_API struct imm_hmm_block*   imm_model_get_hmm_block(struct imm_model const* model, uint8_t i);
IMM_API uint8_t                 imm_model_nhmm_blocks(struct imm_model const* model);
IMM_API void                    imm_model_destroy(struct imm_model const* model);
IMM_API struct imm_model const* imm_model_read(FILE* stream);
IMM_API int                     imm_model_write(struct imm_model const* model, FILE* stream);

IMM_API struct imm_model* __imm_model_create(struct imm_hmm* hmm, struct imm_dp const* dp,
                                             imm_model_read_state_cb  read_state,
                                             void*                    read_state_args,
                                             imm_model_write_state_cb write_state,
                                             void*                    write_state_args);
IMM_API void              __imm_model_deep_destroy(struct imm_model const* model);
IMM_API struct imm_model* __imm_model_new(imm_model_read_state_cb read_state, void* read_state_args,
                                          imm_model_write_state_cb write_state,
                                          void*                    write_state_args);
IMM_API int __imm_model_read_dp(struct imm_model* model, struct imm_hmm_block* block, FILE* stream);
IMM_API int __imm_model_read_hmm(struct imm_model* model, struct imm_hmm_block* block,
                                 FILE* stream);
IMM_API struct imm_state const* __imm_model_read_state(struct imm_model const* model, FILE* stream,
                                                       void* args);
IMM_API void __imm_model_set_abc(struct imm_model* model, struct imm_abc const* abc);
IMM_API int  __imm_model_write_dp(struct imm_model const* model, struct imm_hmm_block const* block,
                                  FILE* stream);
IMM_API int  __imm_model_write_hmm(struct imm_model const* model, struct imm_hmm_block const* block,
                                   FILE* stream);
IMM_API int  __imm_model_write_state(struct imm_model const* model, FILE* stream,
                                     struct imm_state const* state, void* args);

#endif
