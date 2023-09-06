#include "task.h"
#include "dp.h"
#include "eseq.h"
#include "matrix.h"
#include "seq.h"
#include <stdio.h>
#include <stdlib.h>

struct imm_task *imm_task_new(struct imm_dp const *dp)
{
  struct imm_task *x = malloc(sizeof(*x));
  if (!x) return NULL;
  if (imm_matrix_init(&x->matrix, &dp->state_table))
  {
    free(x);
    return NULL;
  }
  x->code = dp->code;
  x->seq = NULL;
  x->num_states = dp->state_table.nstates;
  imm_trellis_init(&x->trellis, &dp->state_table);
  return x;
}

int imm_task_reset(struct imm_task *x, struct imm_dp const *dp)
{
  x->seq = NULL;
  x->num_states = dp->state_table.nstates;
  return imm_matrix_reset(&x->matrix, &dp->state_table);
}

void imm_task_prepare(struct imm_task *x) { imm_matrix_prepare(&x->matrix); }

int imm_task_setup(struct imm_task *x, struct imm_eseq const *seq)
{
  x->seq = seq;
  return imm_trellis_setup(&x->trellis, imm_eseq_size(seq), x->num_states);
}

void imm_task_del(struct imm_task const *x)
{
  if (x)
  {
    imm_matrix_cleanup((struct imm_matrix *)&x->matrix);
    imm_trellis_cleanup((struct imm_trellis *)&x->trellis);
    free((void *)x);
  }
}

void imm_task_dump(struct imm_task const *x, FILE *restrict fp)
{
  imm_matrix_dump(&x->matrix, fp);
}
