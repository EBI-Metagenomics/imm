#include "task.h"
#include "cpath.h"
#include "dp.h"
#include "eseq.h"
#include "matrix.h"
#include "seq.h"
#include <stdio.h>
#include <stdlib.h>

struct imm_task *imm_task_new(struct imm_dp const *dp)
{
  struct imm_task *task = malloc(sizeof(*task));
  if (!task) return NULL;
  if (imm_matrix_init(&task->matrix, &dp->state_table))
  {
    free(task);
    return NULL;
  }
  if (imm_cpath_init(&task->path, &dp->state_table, &dp->trans_table))
  {
    imm_matrix_cleanup(&task->matrix);
    free(task);
    return NULL;
  }
  task->code = dp->code;
  task->seq = NULL;
  imm_trellis_init(&task->trellis, &dp->state_table, &dp->trans_table);
  return task;
}

int imm_task_reset(struct imm_task *x, struct imm_dp const *dp)
{
  int rc = imm_matrix_reset(&x->matrix, &dp->state_table);
  if (rc) return rc;
  /* TODO: memory leak possibility */
  if ((rc = imm_cpath_reset(&x->path, &dp->state_table, &dp->trans_table)))
    return rc;
  x->seq = NULL;
  return 0;
}

int imm_task_setup(struct imm_task *x, struct imm_eseq const *seq)
{
  x->seq = seq;
  // TODO: memory leak
  int rc = imm_cpath_setup(&x->path, imm_eseq_size(seq) + 1);
  if (rc) return rc;
  // TODO: memory leak
  rc = imm_trellis_setup(&x->trellis, imm_eseq_size(seq) + 1, x->path.nstates);
  if (rc) return rc;
  return rc;
}

void imm_task_del(struct imm_task const *x)
{
  if (x)
  {
    imm_matrix_cleanup((struct imm_matrix *)&x->matrix);
    imm_cpath_cleanup((struct imm_cpath *)&x->path);
    imm_trellis_cleanup((struct imm_trellis *)&x->trellis);
    free((void *)x);
  }
}
