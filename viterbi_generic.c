#include "viterbi_generic.h"
#include "lprob.h"
#include "max.h"
#include "range.h"
#include "remains.h"
#include "state_range.h"
#include "tardy_state.h"
#include "viterbi.h"

struct step
{
  uint_fast16_t src_idx;
  uint_fast16_t src_trans;
  uint_fast8_t src_seqlen;
  float score;
};

static inline struct step step_init(void)
{
  return (struct step){IMM_STATE_NULL_IDX, IMM_TRANS_NULL_IDX,
                       IMM_STATE_NULL_SEQLEN, IMM_LPROB_ZERO};
}

TEMPLATE struct imm_ctrans const *
viterbi_best_incoming(struct step *best_step, struct imm_viterbi const *x,
                      unsigned const row, uint_fast16_t const dst,
                      struct imm_ctrans const *ctrans, bool const tardy_state,
                      bool const safe_past)
{
  struct step step = step_init();
  unsigned trans = 0;

  while (dst == ctrans->dst)
  {
    struct state_range src = imm_viterbi_state_range(x, ctrans->src);
    float const tscore = ctrans->score;

    if (tardy_state) src.min = imm_max(src.min, 1U);
    if (!safe_past) src.max = imm_min(src.max, row);

    float tf = IMM_LPROB_ZERO;
    uint_fast8_t src_seqlen = 0;

    imm_assume(src.max <= IMM_STATE_MAX_SEQLEN);
    UNROLL(IMM_STATE_MAX_SEQLEN + 1)
    for (uint_fast8_t i = src.min; i <= src.max; ++i)
    {
      imm_assume(row >= i);
      imm_assume(i >= src.min);
      float v = imm_viterbi_get_score(x, imm_cell(row - i, src.idx, i));
      if (tf < v)
      {
        src_seqlen = i - src.min;
        tf = v;
      }
    }
    tf += tscore;

    if (step.score < tf)
    {
      step.src_idx = src.idx;
      step.src_trans = trans;
      step.src_seqlen = src_seqlen;
      step.score = tf;
    }

    ++trans;
    ++ctrans;
  }

  *best_step = step;
  return ctrans;
}

TEMPLATE void set_path(struct imm_cpath *x, struct step const *bt,
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
                              struct step const *bt, bool const safe_future)
{
  float score = bt->score;
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
         bool const safe_future, bool const safe_past, bool const tardy_state)
{
  struct step bt = {0};
  unsigned const remain = remains(x->seqlen, row, safe_future);

  t = viterbi_best_incoming(&bt, x, row, dst.idx, t, tardy_state, safe_past);
  set_state_score(x, row, dst, remain, &bt, safe_future);
  return t;
}

TEMPLATE void on_tardy_state(struct imm_viterbi const *x, unsigned const row,
                             struct tardy_state const tardy,
                             bool const safe_future, bool const safe_past)
{
  struct imm_ctrans const *t = x->dp->trans_table.trans + tardy.trans_start;
  struct state_range dst = imm_viterbi_state_range(x, tardy.state_idx);
  on_state(x, row, dst, t, safe_future, safe_past, true);
}

TEMPLATE struct imm_ctrans const *
on_states(struct imm_ctrans const *t, struct imm_range const states,
          struct imm_viterbi const *x, unsigned const row,
          bool const safe_future, bool const safe_past)
{
  for (unsigned i = states.a; i < states.b; ++i)
  {
    struct state_range dst = imm_viterbi_state_range(x, i);
    t = on_state(x, row, dst, t, safe_future, safe_past, false);
  }
  return t;
}

TEMPLATE void on_row(struct imm_viterbi const *x, unsigned const row,
                     bool const safe_future, bool const safe_past)
{
  if (x->has_tardy_state)
    on_tardy_state(x, row, x->tardy_state, safe_future, safe_past);
  struct imm_ctrans const *t = x->dp->trans_table.trans;
  struct imm_range const r = imm_range(0, x->dp->state_table.nstates);
  on_states(t, r, x, row, safe_future, safe_past);
}

void imm_viterbi_generic(struct imm_viterbi const *x)
{
  struct imm_dp_safety const *y = &x->safety;

  on_row(x, 0, false, false);

  for (unsigned r = y->safe_future.a; r < y->safe_future.b; ++r)
    on_row(x, r, true, false);

  for (unsigned r = y->safe.a; r < y->safe.b; ++r)
    on_row(x, r, true, true);

  for (unsigned r = y->unsafe.a; r < y->unsafe.b; ++r)
    on_row(x, r, false, false);

  for (unsigned r = y->safe_past.a; r < y->safe_past.b; ++r)
    on_row(x, r, false, true);
}
