#ifndef IMM_MODEL_H
#define IMM_MODEL_H

#include "imm/export.h"
#include <inttypes.h>
#include <stdio.h>

struct imm_model;
struct imm_hmm;
struct imm_dp;

struct imm_model_vtable
{
    void (*destroy)(struct imm_model const* model);
    void (*destroy_on_read_failure)(struct imm_model const* model);
    struct imm_state const* (*read_state)(struct imm_model const* model, FILE* stream,
                                          uint8_t type_id);
    int (*write)(struct imm_model const* model, FILE* stream);
};

IMM_API struct imm_abc const*   imm_model_abc(struct imm_model const* model);
IMM_API struct imm_model*       imm_model_create(struct imm_hmm* hmm, struct imm_dp const* dp);
IMM_API void                    imm_model_destroy(struct imm_model const* model);
IMM_API struct imm_dp const*    imm_model_dp(struct imm_model const* model);
IMM_API struct imm_hmm*         imm_model_hmm(struct imm_model const* model);
IMM_API uint32_t                imm_model_nstates(struct imm_model const* model);
IMM_API struct imm_state const* imm_model_state(struct imm_model const* model, uint32_t i);

IMM_API struct imm_model*       __imm_model_create(struct imm_hmm* hmm, struct imm_dp const* dp,
                                                   void* derived);
IMM_API void*                   __imm_model_derived(struct imm_model const* model);
IMM_API void                    __imm_model_destroy_on_read_failure(struct imm_model const* model);
IMM_API struct imm_model*       __imm_model_new(void* derived);
IMM_API int                     __imm_model_read(struct imm_model* model, FILE* stream);
IMM_API int                     __imm_model_read_abc(struct imm_model* model, FILE* stream);
IMM_API int                     __imm_model_read_dp(struct imm_model* model, FILE* stream);
IMM_API int                     __imm_model_read_hmm(struct imm_model* model, FILE* stream);
IMM_API struct imm_state const* __imm_model_read_state(struct imm_model const* model, FILE* stream,
                                                       uint8_t type_id);
IMM_API void __imm_model_set_abc(struct imm_model* model, struct imm_abc const* abc);
IMM_API struct imm_model_vtable* __imm_model_vtable(struct imm_model* model);
IMM_API int                      __imm_model_write_abc(struct imm_model const* model, FILE* stream);
IMM_API int                      __imm_model_write_dp(struct imm_model const* model, FILE* stream);
IMM_API int                      __imm_model_write_hmm(struct imm_model const* model, FILE* stream);

#endif
