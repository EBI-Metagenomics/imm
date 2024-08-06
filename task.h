#ifndef IMM_TASK_H
#define IMM_TASK_H

#include "compiler.h"
#include "eseq.h"
#include "matrix.h"
#include "rc.h"
#include "state_table.h"
#include "trellis.h"
#include <stdbool.h>

struct imm_dp;
struct imm_seq;
struct imm_eseq;

struct imm_task
{
  struct imm_matrix matrix;
  struct imm_code const *code;
  struct imm_eseq const *seq;
  int num_states;
  struct imm_trellis trellis;

  // Debugging purpose
  struct
  {
    char const *seq;
  } debug;
};

struct imm_task *imm_task_new(struct imm_dp const *);
int imm_task_reset(struct imm_task *, struct imm_dp const *);
void imm_task_prepare(struct imm_task *);
int imm_task_setup(struct imm_task *, struct imm_eseq const *);
void imm_task_del(struct imm_task const *);
void imm_task_dump(struct imm_task const *, FILE *restrict);
void imm_task_debug_setup(struct imm_task *, char const *seq);

#endif
