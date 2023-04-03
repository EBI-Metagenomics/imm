#include "dp_safety.h"
#include "state.h"
#include <assert.h>

// Define as:
//   lowest row such that r >= max(max_seq) > 0 for every r in [row, seqlen].
static struct imm_range safe_past(unsigned seqlen)
{
  assert(IMM_STATE_MAX_SEQLEN > 0);
  if (seqlen >= IMM_STATE_MAX_SEQLEN)
    return imm_range(IMM_STATE_MAX_SEQLEN, seqlen + 1);
  return imm_range(seqlen + 1, seqlen + 1);
}

// Define as:
//   greatest row such that seqlen >= max(max_seq) + r for every
//   r in (0, row].
static struct imm_range safe_future(unsigned seqlen)
{
  if (seqlen >= IMM_STATE_MAX_SEQLEN)
    return imm_range(1, seqlen - IMM_STATE_MAX_SEQLEN + 1);
  return imm_range(1, 1);
}

void imm_dp_safety_init(struct imm_dp_safety *x, unsigned len)
{
  struct imm_range past = safe_past(len);
  struct imm_range future = safe_future(len);
  x->safe = imm_range_intersect(past, future);

  struct imm_range tmp = {0};

  imm_range_subtract(future, x->safe, &x->safe_future, &tmp);
  if (imm_range_empty(x->safe_future)) imm_range_swap(&x->safe_future, &tmp);
  assert(imm_range_empty(tmp));

  imm_range_subtract(past, x->safe, &tmp, &x->safe_past);
  if (imm_range_empty(x->safe_past)) imm_range_swap(&x->safe_past, &tmp);
  assert(imm_range_empty(tmp));

  if (imm_range_empty(x->safe))
    imm_range_set(&x->unsafe, x->safe_future.b, x->safe_past.a);
  else
    imm_range_set(&x->unsafe, x->safe.b, x->safe.b);

  assert(imm_range_empty(x->safe) || imm_range_empty(x->unsafe));
}
