#ifndef IMM_VITERBI_H
#define IMM_VITERBI_H

#include "dp_safety.h"
#include "export.h"
#include "tardy_state.h"
#include <stdbool.h>

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

#endif
