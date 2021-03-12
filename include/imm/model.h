#ifndef IMM_MODEL_H
#define IMM_MODEL_H

#include "imm/export.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

struct imm_dp;
struct imm_hmm;
struct imm_model;

IMM_API struct imm_model*       imm_model_create(struct imm_hmm* hmm, struct imm_dp const* dp);
IMM_API void                    imm_model_destroy(struct imm_model const* model, bool deep);
IMM_API struct imm_dp*          imm_model_dp(struct imm_model const* model);
IMM_API struct imm_hmm*         imm_model_hmm(struct imm_model const* model);
IMM_API uint16_t                imm_model_nstates(struct imm_model const* model);
IMM_API struct imm_state const* imm_model_state(struct imm_model const* model, uint16_t i);

#endif
