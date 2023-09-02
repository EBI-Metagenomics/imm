#include "viterbi.h"
#include "assume.h"
#include "cell.h"
#include "compiler.h"
#include "dp.h"
#include "lprob.h"
#include "matrix.h"
#include "max.h"
#include "range.h"
#include "span.h"
#include "state_table.h"
#include "task.h"
#include "trans_table.h"
#include "unroll.h"
#include "zspan.h"

imm_pure_template float start_lprob(struct imm_viterbi const *x)
{
  return x->dp->state_table.start.lprob;
}

imm_pure_template unsigned start_state_idx(struct imm_viterbi const *x)
{
  return x->dp->state_table.start.state_idx;
}

imm_pure_template uint16_t start_trans_idx(struct imm_viterbi const *x,
                                           unsigned state_idx)
{
  return imm_trans_table_trans_start(&x->dp->trans_table, state_idx);
}

imm_template void viterbi_ctrans_rewind(struct imm_viterbi *x)
{
  x->curr_trans = imm_trans_table_ctrans_start(&x->dp->trans_table);
}

imm_template void viterbi_ctrans_seek(struct imm_viterbi *x, unsigned trans_idx)
{
  x->curr_trans = x->dp->trans_table.trans + trans_idx;
}

imm_template void viterbi_ctrans_next(struct imm_viterbi *x)
{
  x->curr_trans++;
}

imm_template void set_matrix_cell_score(struct imm_viterbi const *x,
                                        struct imm_cell cell, float score)
{
  imm_matrix_set_score(&x->task->matrix, cell, score);
}

imm_pure_template float get_matrix_cell_score(struct imm_viterbi const *x,
                                              unsigned row, uint_fast16_t src,
                                              uint_fast8_t emission_length)
{
  imm_assume(row >= emission_length);
  struct imm_cell cell = imm_cell(row - emission_length, src, emission_length);
  return imm_matrix_get_score(&x->task->matrix, cell);
}

imm_pure_template unsigned nstates(struct imm_viterbi const *x)
{
  return x->dp->state_table.nstates;
}

struct state
{
  uint_fast16_t idx;
  uint_fast8_t min;
  uint_fast8_t max;
};

imm_pure_template uint8_t state_zspan(struct imm_viterbi const *x,
                                      unsigned state_idx)
{
  return imm_state_table_zspan(&x->dp->state_table, state_idx);
}

imm_pure_template struct state unwrap_state(struct imm_viterbi const *x,
                                            unsigned state_idx)
{
  uint8_t span = state_zspan(x, state_idx);
  imm_assume(imm_zspan_min(span) <= IMM_STATE_MAX_SEQLEN);
  imm_assume(imm_zspan_max(span) <= IMM_STATE_MAX_SEQLEN);
  imm_assume(imm_zspan_min(span) <= imm_zspan_max(span));
  return (struct state){state_idx, imm_zspan_min(span), imm_zspan_max(span)};
}

static void find_tardy_states(struct imm_viterbi *, struct imm_dp const *);

void imm_viterbi_init(struct imm_viterbi *x, struct imm_dp const *dp,
                      struct imm_task *task)
{
  x->dp = dp;
  x->task = task;
  x->curr_trans = NULL;
  viterbi_ctrans_rewind(x);
  imm_dp_safety_init(&x->safety, imm_eseq_size(task->seq));
  x->seqlen = imm_eseq_size(task->seq);
  find_tardy_states(x, dp);
}

imm_pure_template unsigned ntrans(struct imm_viterbi const *x, unsigned dst)
{
  return imm_trans_table_ntrans(&x->dp->trans_table, dst);
}

static void find_tardy_states(struct imm_viterbi *x, struct imm_dp const *dp)
{
  unsigned n = 0;
  unsigned state_idx = 0;
  for (unsigned dst = 0; dst < dp->state_table.nstates; ++dst)
  {
    for (unsigned t = 0; t < ntrans(x, dst); ++t)
    {
      unsigned src = imm_trans_table_source_state(&dp->trans_table, dst, t);
      uint8_t span = state_zspan(x, src);
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
  unsigned trans_idx = start_trans_idx(x, state_idx);

  x->has_tardy_state = n > 0;
  x->tardy_state = (struct tardy_state){state_idx, trans_idx};
}

struct step
{
  uint_fast16_t src_idx;
  uint_fast16_t src_trans;
  uint_fast8_t src_emissize;
  uint_fast8_t src_seqlen_idx;
  float score;
};

static inline struct step step_init(void)
{
  return (struct step){IMM_STATE_NULL_IDX, IMM_TRANS_NULL_IDX,
                       IMM_STATE_NULL_SEQLEN, IMM_STATE_NULL_SEQLEN,
                       IMM_LPROB_ZERO};
}

imm_template struct step best_step(struct imm_viterbi *x, unsigned row,
                                   uint_fast16_t dst, bool tardy_state,
                                   bool safe_past)
{
  struct step step = step_init();
  unsigned trans = 0;

  while (dst == x->curr_trans->dst)
  {
    struct state src = unwrap_state(x, x->curr_trans->src);

    if (tardy_state) src.min = imm_max(src.min, 1U);
    if (!safe_past) src.max = imm_min(src.max, row);

    float score = IMM_LPROB_ZERO;
    uint_fast8_t emissize = 0;
    uint_fast8_t seqlen_idx = 0;

    imm_assume(src.max <= IMM_STATE_MAX_SEQLEN);
    UNROLL(IMM_STATE_MAX_SEQLEN + 1)
    for (uint_fast8_t i = src.min; i <= src.max; ++i)
    {
      float v = get_matrix_cell_score(x, row, src.idx, i);
      if (score < v)
      {
        emissize = i;
        seqlen_idx = i - src.min;
        score = v;
      }
    }
    score += x->curr_trans->score;

    if (step.score < score)
    {
      step.src_idx = src.idx;
      step.src_trans = trans;
      step.src_emissize = emissize;
      step.src_seqlen_idx = seqlen_idx;
      step.score = score;
    }

    ++trans;
    viterbi_ctrans_next(x);
  }

  return step;
}

imm_template void set_path(struct imm_cpath *x, struct step const *bt,
                           unsigned r, uint_fast16_t dst)
{
  if (bt->src_idx != IMM_STATE_NULL_IDX)
  {
    imm_cpath_set_trans(x, r, dst, bt->src_trans);
    imm_cpath_set_seqlen_idx(x, r, dst, bt->src_seqlen_idx);
    imm_cpath_set_score(x, r, dst, bt->score);
    assert(imm_cpath_trans(x, r, dst) == bt->src_trans);
    assert(imm_cpath_seqlen_idx(x, r, dst) == bt->src_seqlen_idx);
  }
  else
  {
    imm_cpath_invalidate(x, r, dst);
    assert(!imm_cpath_valid(x, r, dst));
  }
}

imm_template void set_trellis(struct imm_trellis *x, float score,
                              struct step const *bt, unsigned r,
                              uint_fast16_t dst)
{
  if (bt->src_idx != IMM_STATE_NULL_IDX)
  {
    (void)r;
    (void)dst;
    assert(r == imm_trellis_sequence_idx(x));
    assert(dst == imm_trellis_state_idx(x));
    imm_trellis_push(x, score, bt->src_idx, bt->src_emissize);
  }
  else imm_trellis_push(x, IMM_LPROB_NAN, 0, 0);
}

imm_pure_template float emission_score(struct imm_viterbi const *x,
                                       struct state state, unsigned row,
                                       unsigned len)
{
  unsigned code = imm_eseq_get(x->task->seq, row, len, state.min);
  return imm_emis_score(&x->dp->emis, state.idx, code);
}

imm_template void set_state_score(struct imm_viterbi const *x, unsigned row,
                                  struct state dst, unsigned remain,
                                  struct step const *bt, bool safe_future)
{
  float score = bt->score;
  if (row == 0 && start_state_idx(x) == dst.idx)
    score = imm_max(start_lprob(x), score);

  set_path(&x->task->path, bt, row, dst.idx);
  set_trellis(&x->task->trellis, score, bt, row, dst.idx);
  if (!safe_future) dst.max = imm_min(dst.max, remain);

  imm_assume(dst.max <= IMM_STATE_MAX_SEQLEN);

  UNROLL(IMM_STATE_MAX_SEQLEN + 1)
  for (uint_fast8_t i = dst.min; i <= dst.max; ++i)
  {
    float total = score + emission_score(x, dst, row, i);
    set_matrix_cell_score(x, imm_cell(row, dst.idx, i), total);
  }
}

imm_const_template unsigned remains(unsigned seqlen, unsigned row,
                                    bool safe_future)
{
  if (safe_future) return IMM_STATE_MAX_SEQLEN;
  if (seqlen >= IMM_STATE_MAX_SEQLEN + row) return IMM_STATE_MAX_SEQLEN;
  return seqlen - row;
}

imm_template void on_normal_state(struct imm_viterbi *x, unsigned row,
                                  struct state dst, bool safe_future,
                                  bool safe_past, bool tardy_state)
{
  unsigned remain = remains(x->seqlen, row, safe_future);

  struct step step = best_step(x, row, dst.idx, tardy_state, safe_past);
  set_state_score(x, row, dst, remain, &step, safe_future);
}

imm_template void on_tardy_state(struct imm_viterbi *x, unsigned row,
                                 struct tardy_state const tardy,
                                 bool safe_future, bool safe_past)
{
  viterbi_ctrans_seek(x, tardy.trans_start);
  struct state dst = unwrap_state(x, tardy.state_idx);
  on_normal_state(x, row, dst, safe_future, safe_past, true);
}

imm_template void on_row(struct imm_viterbi *x, unsigned row,
                         bool has_tardy_state, bool safe_future, bool safe_past)
{
  if (has_tardy_state)
  {
    imm_trellis_seek(&x->task->trellis, row, x->tardy_state.state_idx);
    on_tardy_state(x, row, x->tardy_state, safe_future, safe_past);
  }

  imm_trellis_seek(&x->task->trellis, row, 0);
  viterbi_ctrans_rewind(x);

  for (unsigned i = 0; i < nstates(x); ++i)
  {
    struct state dst = unwrap_state(x, i);
    on_normal_state(x, row, dst, safe_future, safe_past, false);
  }
}

imm_template void viterbi_generic(struct imm_viterbi *x, bool has_tardy_state)
{
  struct imm_dp_safety const *y = &x->safety;

  on_row(x, 0, has_tardy_state, false, false);

  for (unsigned r = y->safe_future.start; r < y->safe_future.stop; ++r)
    on_row(x, r, has_tardy_state, true, false);

  for (unsigned r = y->safe.start; r < y->safe.stop; ++r)
    on_row(x, r, has_tardy_state, true, true);

  for (unsigned r = y->unsafe.start; r < y->unsafe.stop; ++r)
    on_row(x, r, has_tardy_state, false, false);

  for (unsigned r = y->safe_past.start; r < y->safe_past.stop; ++r)
    on_row(x, r, has_tardy_state, false, true);
}

void imm_viterbi_run(struct imm_viterbi *x)
{
  if (x->has_tardy_state) viterbi_generic(x, true);
  else viterbi_generic(x, false);
}
