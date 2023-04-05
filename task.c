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
  if (!task)
  {
    // error(IMM_NOMEM);
    return NULL;
  }
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
  imm_eseq_init(&task->eseq, dp->code);
  task->seq = NULL;
  task->save_path = true;
  return task;
}

int imm_task_reset(struct imm_task *x, struct imm_dp const *dp)
{
  int rc = imm_matrix_reset(&x->matrix, &dp->state_table);
  if (rc) return rc;
  /* TODO: memory leak possibility */
  if ((rc = imm_cpath_reset(&x->path, &dp->state_table, &dp->trans_table)))
    return rc;
  imm_eseq_reset(&x->eseq, dp->code);
  x->seq = NULL;
  return 0;
}

int imm_task_setup(struct imm_task *x, struct imm_seq const *seq)
{
  x->seq = seq;
  int rc = imm_cpath_setup(&x->path, imm_seq_size(seq) + 1);
  if (rc) return rc;
  return imm_eseq_setup(&x->eseq, seq);
}

void imm_task_set_save_path(struct imm_task *x, bool save)
{
  x->save_path = save;
}

void imm_task_del(struct imm_task const *x)
{
  if (x)
  {
    imm_matrix_cleanup((struct imm_matrix *)&x->matrix);
    imm_cpath_cleanup((struct imm_cpath *)&x->path);
    imm_eseq_cleanup(&x->eseq);
    free((void *)x);
  }
}