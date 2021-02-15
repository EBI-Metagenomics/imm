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

struct imm_model_vtable
{
    struct imm_state const* (*read_state)(struct imm_model * model, FILE* stream);
    int (*write_state)(struct imm_model const* model, FILE* stream, struct imm_state const* state);
};

IMM_API struct imm_abc const* imm_model_abc(struct imm_model const* model);
IMM_API struct imm_model*     imm_model_create(void);
IMM_API void imm_model_append_hmm_block(struct imm_model* model, struct imm_hmm* hmm,
                                        struct imm_dp const* dp);
IMM_API struct imm_hmm_block*   imm_model_get_hmm_block(struct imm_model const* model, uint8_t i);
IMM_API uint8_t                 imm_model_nhmm_blocks(struct imm_model const* model);
IMM_API void                    imm_model_destroy(struct imm_model const* model);
IMM_API struct imm_model const* imm_model_read(FILE* stream);
IMM_API int                     imm_model_write(struct imm_model const* model, FILE* stream);

IMM_API int               __imm_model_read_hmm_blocks(struct imm_model* model, FILE* stream);
IMM_API int               __imm_model_write_hmm_blocks(struct imm_model const* model, FILE* stream);
IMM_API struct imm_model* __imm_model_create(struct imm_model_vtable vtable, void* derived);
IMM_API void              __imm_model_deep_destroy(struct imm_model const* model);
IMM_API void              __imm_model_set_abc(struct imm_model* model, struct imm_abc const* abc);
IMM_API void*             __imm_model_derived(struct imm_model* model);
IMM_API void const*             __imm_model_derived_c(struct imm_model const* model);

#endif
