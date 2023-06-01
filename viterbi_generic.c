#include "viterbi_generic.h"
#include "hot_range.h"
#include "range.h"
#include "remains.h"
#include "state_range.h"
#include "unsafe.h"
#include "viterbi.h"
#include "viterbi_any.h"
#include "viterbi_best_incoming.h"

static void viterbi_gany(struct imm_viterbi const *x, unsigned const seqlen,
                         struct unsafe unsafe, struct imm_dp_safety const *y,
                         bool const save_path)
{
  gany(x, 0, unsafe, seqlen, save_path, false, false);

  for (unsigned r = y->safe_future.a; r < y->safe_future.b; ++r)
    gany(x, r, unsafe, seqlen, save_path, true, false);

  for (unsigned r = y->safe.a; r < y->safe.b; ++r)
    gany(x, r, unsafe, seqlen, save_path, true, true);

  for (unsigned r = y->unsafe.a; r < y->unsafe.b; ++r)
    gany(x, r, unsafe, seqlen, save_path, false, false);

  for (unsigned r = y->safe_past.a; r < y->safe_past.b; ++r)
    gany(x, r, unsafe, seqlen, save_path, false, true);
}

void imm_viterbi_generic(struct imm_viterbi const *x, unsigned const seqlen)
{
  struct unsafe const unsafe = unsafe_init(x);
  struct imm_dp_safety const *y = &x->safety;
  bool const save_path = x->task->save_path;
  viterbi_gany(x, seqlen, unsafe, y, save_path);
}
