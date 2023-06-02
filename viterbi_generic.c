#include "viterbi_generic.h"
#include "range.h"
#include "remains.h"
#include "state_range.h"
#include "tardy_state.h"
#include "viterbi.h"
#include "viterbi_best_incoming.h"

TEMPLATE void set_path(struct imm_cpath *x, struct viterbi_best_trans const *bt,
                       unsigned const r, uint_fast16_t const dst)
{
  if (bt->src_idx != IMM_STATE_NULL_IDX)
  {
    imm_cpath_set_trans(x, r, dst, bt->src_trans);
    imm_cpath_set_seqlen(x, r, dst, bt->src_seqlen);
    assert(imm_cpath_trans(x, r, dst) == bt->src_trans);
    assert(imm_cpath_seqlen(x, r, dst) == bt->src_seqlen);
  }
  else
  {
    imm_cpath_invalidate(x, r, dst);
    assert(!imm_cpath_valid(x, r, dst));
  }
}

TEMPLATE void set_state_score(struct imm_viterbi const *x, unsigned const row,
                              struct state_range dst, unsigned const remain,
                              float score, struct viterbi_best_trans const *bt,
                              bool const safe_future)
{
  if (row == 0 && imm_viterbi_start_state(x) == dst.idx)
    score = imm_max(imm_viterbi_start_lprob(x), score);

  set_path(&x->task->path, bt, row, dst.idx);
  if (!safe_future) dst.max = imm_min(dst.max, remain);

  imm_assume(dst.max <= IMM_STATE_MAX_SEQLEN);

  UNROLL(IMM_STATE_MAX_SEQLEN + 1)
  for (uint_fast8_t i = dst.min; i <= dst.max; ++i)
  {
    float total = score + imm_viterbi_emission(x, row, dst.idx, i, dst.min);
    imm_viterbi_set_score(x, imm_cell(row, dst.idx, i), total);
  }
}

TEMPLATE struct imm_ctrans const *
on_state(struct imm_viterbi const *x, unsigned const row,
         struct state_range dst, struct imm_ctrans const *t,
         unsigned const seqlen, bool const safe_future, bool const safe_past,
         bool const unsafe_state)
{
  float score = 0;
  struct viterbi_best_trans bt = {0};
  unsigned const remain = remains(seqlen, row, safe_future);

  t = viterbi_best_incoming(&score, &bt, x, row, dst.idx, t, unsafe_state,
                            safe_past);
  set_state_score(x, row, dst, remain, score, &bt, safe_future);
  return t;
}

TEMPLATE void on_tardy_state(struct imm_viterbi const *x, unsigned const row,
                             struct tardy_state const tardy,
                             unsigned const seqlen, bool const safe_future,
                             bool const safe_past)
{
  struct imm_ctrans const *t = x->dp->trans_table.trans + tardy.trans_start;
  struct state_range dst = imm_viterbi_state_range(x, tardy.state_idx);
  on_state(x, row, dst, t, seqlen, safe_future, safe_past, true);
}

TEMPLATE struct imm_ctrans const *
on_states(struct imm_ctrans const *t, struct imm_range const states,
          struct imm_viterbi const *x, unsigned const row,
          unsigned const seqlen, bool const safe_future, bool const safe_past)
{
  for (unsigned i = states.a; i < states.b; ++i)
  {
    struct state_range dst = imm_viterbi_state_range(x, i);
    t = on_state(x, row, dst, t, seqlen, safe_future, safe_past, false);
  }
  return t;
}

TEMPLATE void on_row(struct imm_viterbi const *x, unsigned const row,
                     unsigned const seqlen, bool const safe_future,
                     bool const safe_past)
{
  if (x->has_tardy_state)
    on_tardy_state(x, row, x->tardy_state, seqlen, safe_future, safe_past);
  struct imm_ctrans const *t = x->dp->trans_table.trans;
  struct imm_range const r = imm_range(0, x->dp->state_table.nstates);
  on_states(t, r, x, row, seqlen, safe_future, safe_past);
}

void imm_viterbi_generic(struct imm_viterbi const *x, unsigned const seqlen)
{
  struct imm_dp_safety const *y = &x->safety;

  on_row(x, 0, seqlen, false, false);

  for (unsigned r = y->safe_future.a; r < y->safe_future.b; ++r)
    on_row(x, r, seqlen, true, false);

  for (unsigned r = y->safe.a; r < y->safe.b; ++r)
    on_row(x, r, seqlen, true, true);

  for (unsigned r = y->unsafe.a; r < y->unsafe.b; ++r)
    on_row(x, r, seqlen, false, false);

  for (unsigned r = y->safe_past.a; r < y->safe_past.b; ++r)
    on_row(x, r, seqlen, false, true);
}
