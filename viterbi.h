#ifndef IMM_VITERBI_H
#define IMM_VITERBI_H

#include "assume.h"
#include "cell.h"
#include "compiler.h"
#include "dp.h"
#include "dp_safety.h"
#include "range.h"
#include "tardy_state.h"
#include "task.h"
#include "trans_table.h"
#include "zspan.h"

struct imm_dp;
struct imm_task;

struct imm_viterbi
{
  struct imm_dp const *dp;
  struct imm_task *task;
  struct imm_dp_safety safety;
  unsigned seqlen;
  bool has_tardy_state;
  struct tardy_state tardy_state;
};

IMM_API void imm_viterbi_init(struct imm_viterbi *, struct imm_dp const *,
                              struct imm_task *);
IMM_API void imm_viterbi_run(struct imm_viterbi const *);

struct imm_range imm_viterbi_range(struct imm_viterbi const *,
                                   unsigned state_idx);
float imm_viterbi_start_lprob(struct imm_viterbi const *);

imm_pure_template unsigned
imm_viterbi_start_state_idx(struct imm_viterbi const *x)
{
  return x->dp->state_table.start.state_idx;
}

#endif
