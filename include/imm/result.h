#ifndef IMM_RESULT_H
#define IMM_RESULT_H

#include "imm/export.h"
#include "imm/float.h"

struct imm_path;
struct imm_result;
struct imm_seq;

IMM_API struct imm_result*     imm_result_create(struct imm_seq const* seq);
IMM_API void                   imm_result_destroy(struct imm_result const* result);
IMM_API void                   imm_result_free(struct imm_result const* result);
IMM_API struct imm_path const* imm_result_path(struct imm_result const* result);
IMM_API imm_float              imm_result_seconds(struct imm_result const* result);

#endif
