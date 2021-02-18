#ifndef IMM_PROFILE_H
#define IMM_PROFILE_H

#include "imm/export.h"
#include <inttypes.h>
#include <stdio.h>

struct imm_abc;
struct imm_profile;
struct imm_model;

struct imm_profile_vtable
{
    struct imm_state const* (*read_state)(struct imm_profile* prof, FILE* stream);
    int (*write_state)(struct imm_profile const* prof, FILE* stream, struct imm_state const* state);
};

IMM_API struct imm_abc const*     imm_profile_abc(struct imm_profile const* prof);
IMM_API void                      imm_profile_append_model(struct imm_profile* prof, struct imm_model* model);
IMM_API struct imm_profile*       imm_profile_create(struct imm_abc const* abc);
IMM_API void                      imm_profile_destroy(struct imm_profile const* prof);
IMM_API void                      imm_profile_free(struct imm_profile const* prof);
IMM_API struct imm_model*         imm_profile_get_model(struct imm_profile const* prof, uint8_t i);
IMM_API uint8_t                   imm_profile_nmodels(struct imm_profile const* prof);
IMM_API struct imm_profile const* imm_profile_read(FILE* stream);
IMM_API int                       imm_profile_write(struct imm_profile const* prof, FILE* stream);

IMM_API struct imm_profile* __imm_profile_create(struct imm_abc const* abc, struct imm_profile_vtable vtable,
                                                 void* derived);
IMM_API void                __imm_profile_deep_destroy(struct imm_profile const* prof);
IMM_API void*               __imm_profile_derived(struct imm_profile* prof);
IMM_API void const*         __imm_profile_derived_c(struct imm_profile const* prof);
IMM_API int                 __imm_profile_read_models(struct imm_profile* prof, FILE* stream);
IMM_API void                __imm_profile_set_abc(struct imm_profile* prof, struct imm_abc const* abc);
IMM_API int                 __imm_profile_write_models(struct imm_profile const* prof, FILE* stream);

#endif
