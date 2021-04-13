#ifndef IMM_DP_TASK_H
#define IMM_DP_TASK_H

#include "imm/export.h"
#include <stdint.h>

struct imm_dp;
struct imm_dp_task;
struct imm_seq;

IMM_API struct imm_dp_task* imm_dp_task_create(struct imm_dp const* dp);
IMM_API int                 imm_dp_task_setup(struct imm_dp_task* task, struct imm_seq const* seq);
IMM_API void                imm_dp_task_destroy(struct imm_dp_task const* dp_task);

#endif
