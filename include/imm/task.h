#ifndef IMM_TASK_H
#define IMM_TASK_H

#include "imm/export.h"

struct imm_dp;
struct imm_seq;
struct imm_task;

IMM_API struct imm_task *imm_task_new(struct imm_dp const *dp);

IMM_API int imm_task_reset(struct imm_task *task, struct imm_dp const *dp);

IMM_API int imm_task_setup(struct imm_task *task, struct imm_seq const *seq);

IMM_API void imm_task_del(struct imm_task const *task);

#endif
