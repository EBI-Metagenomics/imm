#include "imm_dp_safety.h"
#include "imm_state.h"
#include <assert.h>

// Define as:
//   lowest row such that r >= max(max_seq) > 0 for every r in [row, seqsize].
static struct imm_range safe_past(int seqsize)
{
  assert(IMM_STATE_MAX_SEQSIZE > 0);
  if (seqsize >= IMM_STATE_MAX_SEQSIZE)
    return imm_range(IMM_STATE_MAX_SEQSIZE, seqsize + 1);
  return imm_range(seqsize + 1, seqsize + 1);
}

// Define as:
//   greatest row such that seqsize >= max(max_seq) + r for every
//   r in (0, row].
static struct imm_range safe_future(int seqsize)
{
  if (seqsize >= IMM_STATE_MAX_SEQSIZE)
    return imm_range(1, seqsize - IMM_STATE_MAX_SEQSIZE + 1);
  return imm_range(1, 1);
}

void imm_dp_safety_init(struct imm_dp_safety *x, int size)
{
  struct imm_range past = safe_past(size);
  struct imm_range future = safe_future(size);
  x->safe = imm_range_intersect(past, future);

  struct imm_range tmp = {0};

  imm_range_subtract(future, x->safe, &x->safe_future, &tmp);
  if (imm_range_empty(x->safe_future)) imm_range_swap(&x->safe_future, &tmp);
  assert(imm_range_empty(tmp));

  imm_range_subtract(past, x->safe, &tmp, &x->safe_past);
  if (imm_range_empty(x->safe_past)) imm_range_swap(&x->safe_past, &tmp);
  assert(imm_range_empty(tmp));

  if (imm_range_empty(x->safe))
    imm_range_set(&x->unsafe, x->safe_future.stop, x->safe_past.start);
  else imm_range_set(&x->unsafe, x->safe.stop, x->safe.stop);

  assert(imm_range_empty(x->safe) || imm_range_empty(x->unsafe));
}
