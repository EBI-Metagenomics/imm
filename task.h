#ifndef IMM_TASK_H
#define IMM_TASK_H

#include "cpath.h"
#include "eseq.h"
#include "export.h"
#include "matrix.h"
#include "rc.h"
#include <stdbool.h>

struct imm_dp;
struct imm_seq;

struct imm_task
{
  struct imm_matrix matrix;
  struct imm_cpath path;
  struct imm_eseq eseq;
  struct imm_seq const *seq;
};

IMM_API struct imm_task *imm_task_new(struct imm_dp const *dp);
IMM_API int imm_task_reset(struct imm_task *, struct imm_dp const *dp);
IMM_API int imm_task_setup(struct imm_task *, struct imm_seq const *seq);
IMM_API void imm_task_del(struct imm_task const *);

#endif
