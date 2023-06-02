#include "viterbi.h"
#include "assume.h"
#include "cell.h"
#include "dp.h"
#include "matrix.h"
#include "span.h"
#include "state_table.h"
#include "task.h"
#include "trans_table.h"

static void find_tardy_states(struct imm_viterbi *, struct imm_dp const *);

void imm_viterbi_init(struct imm_viterbi *x, struct imm_dp const *dp,
                      struct imm_task *task)
{
  x->dp = dp;
  x->task = task;
  imm_dp_safety_init(&x->safety, imm_seq_size(task->seq));
  x->seqlen = imm_seq_size(task->seq);
  find_tardy_states(x, dp);
}

struct imm_range imm_viterbi_range(struct imm_viterbi const *x, unsigned state)
{
  return imm_state_table_range(&x->dp->state_table, state);
}

float imm_viterbi_start_lprob(struct imm_viterbi const *x)
{
  return x->dp->state_table.start.lprob;
}

static void find_tardy_states(struct imm_viterbi *x, struct imm_dp const *dp)
{
  unsigned n = 0;
  unsigned state_idx = 0;
  for (unsigned dst = 0; dst < dp->state_table.nstates; ++dst)
  {
    for (unsigned t = 0; t < imm_trans_table_ntrans(&dp->trans_table, dst); ++t)
    {
      unsigned src = imm_trans_table_source_state(&dp->trans_table, dst, t);
      uint8_t span = imm_state_table_span(&dp->state_table, src);
      if (imm_zspan_min(span) == 0 && dst < src)
      {
        n++;
        state_idx = src;
      }
    }
  }

  // We don't support more than 1 tardy state at the moment.
  assert(n <= 1);
  (void)n;
  unsigned trans_idx = imm_viterbi_trans_start(x, state_idx);

  x->has_tardy_state = n > 0;
  x->tardy_state = (struct tardy_state){state_idx, trans_idx};
}
