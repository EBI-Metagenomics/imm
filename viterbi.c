#include "viterbi.h"
#include "assume.h"
#include "cell.h"
#include "dp.h"
#include "matrix.h"
#include "minmax.h"
#include "span.h"
#include "state_table.h"
#include "task.h"
#include "trans_table.h"

static unsigned find_unsafe_state(struct imm_dp const *dp)
{
  unsigned n = 0;
  unsigned state = 0;
  for (unsigned dst = 0; dst < dp->state_table.nstates; ++dst)
  {
    for (unsigned t = 0; t < imm_trans_table_ntrans(&dp->trans_table, dst); ++t)
    {
      unsigned src = imm_trans_table_source_state(&dp->trans_table, dst, t);
      uint8_t span = imm_state_table_span(&dp->state_table, src);
      if (imm_zspan_min(span) == 0 && dst < src)
      {
        n++;
        state = src;
      }
    }
  }
  assert(n <= 1);
  (void)n;
  return state;
}

void imm_viterbi_init(struct imm_viterbi *x, struct imm_dp const *dp,
                      struct imm_task *task)
{
  x->dp = dp;
  x->task = task;
  imm_dp_safety_init(&x->safety, imm_seq_size(task->seq));
  x->seqlen = imm_seq_size(task->seq);
  x->unsafe_state = find_unsafe_state(dp);
}

struct imm_range imm_viterbi_range(struct imm_viterbi const *x, unsigned state)
{
  return imm_state_table_range(&x->dp->state_table, state);
}

unsigned imm_viterbi_start_state(struct imm_viterbi const *x)
{
  return x->dp->state_table.start.state;
}

float imm_viterbi_start_lprob(struct imm_viterbi const *x)
{
  return x->dp->state_table.start.lprob;
}
