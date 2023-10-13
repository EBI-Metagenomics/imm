#include "viterbi.h"
#include "assume.h"
#include "cell.h"
#include "compiler.h"
#include "dp.h"
#include "lprob.h"
#include "matrix.h"
#include "max.h"
#include "min.h"
#include "range.h"
#include "span.h"
#include "state_table.h"
#include "task.h"
#include "trans_table.h"
#include "unroll.h"
#include "zspan.h"
#include <string.h>

IMM_PURE int start_state_idx(struct imm_viterbi const *x)
{
  return x->dp->state_table.start_state_idx;
}

IMM_PURE int16_t start_trans_idx(struct imm_viterbi const *x, int state_idx)
{
  return imm_trans_table_trans_start(&x->dp->trans_table, state_idx);
}

IMM_INLINE void viterbi_ctrans_rewind(struct imm_viterbi *x)
{
  x->curr_trans = imm_trans_table_ctrans_start(&x->dp->trans_table);
}

IMM_INLINE void viterbi_ctrans_seek(struct imm_viterbi *x, int trans_idx)
{
  x->curr_trans = x->dp->trans_table.trans + trans_idx;
}

IMM_INLINE void viterbi_ctrans_next(struct imm_viterbi *x) { x->curr_trans++; }

IMM_INLINE void set_matrix_cell_score(struct imm_viterbi const *x,
                                      struct imm_cell cell, float score)
{
  imm_matrix_set_score(&x->task->matrix, cell, score);
}

IMM_PURE float get_matrix_cell_score(struct imm_viterbi const *x, int row,
                                     int_fast16_t src,
                                     int_fast8_t emission_size)
{
  imm_assume(row >= emission_size);
  struct imm_cell cell = imm_cell(row - emission_size, src, emission_size);
  return imm_matrix_get_score(&x->task->matrix, cell);
}

IMM_PURE int nstates(struct imm_viterbi const *x)
{
  return x->dp->state_table.nstates;
}

struct state
{
  int_fast16_t idx;
  int_fast8_t min;
  int_fast8_t max;
};

IMM_PURE int8_t state_zspan(struct imm_viterbi const *x, int state_idx)
{
  return imm_state_table_zspan(&x->dp->state_table, state_idx);
}

IMM_PURE struct state unwrap_state(struct imm_viterbi const *x, int state_idx)
{
  int8_t span = state_zspan(x, state_idx);
  imm_assume(imm_zspan_min(span) <= IMM_STATE_MAX_SEQSIZE);
  imm_assume(imm_zspan_max(span) <= IMM_STATE_MAX_SEQSIZE);
  imm_assume(imm_zspan_min(span) <= imm_zspan_max(span));
  return (struct state){(int_fast16_t)state_idx, imm_zspan_min(span),
                        imm_zspan_max(span)};
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
  x->seqsize = imm_eseq_size(task->seq);
  find_tardy_states(x, dp);
}

IMM_PURE int ntrans(struct imm_viterbi const *x, int dst)
{
  return imm_trans_table_ntrans(&x->dp->trans_table, dst);
}

static void find_tardy_states(struct imm_viterbi *x, struct imm_dp const *dp)
{
  int n = 0;
  int state_idx = 0;
  for (int dst = 0; dst < dp->state_table.nstates; ++dst)
  {
    for (int t = 0; t < ntrans(x, dst); ++t)
    {
      int src = imm_trans_table_source_state(&dp->trans_table, dst, t);
      int8_t span = state_zspan(x, src);
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
  int trans_idx = start_trans_idx(x, state_idx);

  x->has_tardy_state = n > 0;
  x->tardy_state = (struct tardy_state){state_idx, trans_idx};
}

struct step
{
  int_fast16_t src_idx;
  int_fast16_t src_trans;
  int_fast8_t src_emissize;
  int_fast8_t src_seqsize_idx;
  float emis_score;
  float trans_score;
  float score;
};

static inline struct step step_init(void)
{
  return (struct step){IMM_STATE_NULL_IDX,     IMM_TRANS_NULL_IDX,
                       IMM_STATE_NULL_SEQSIZE, IMM_STATE_NULL_SEQSIZE,
                       IMM_LPROB_ZERO,         IMM_LPROB_ZERO,
                       IMM_LPROB_ZERO};
}

IMM_INLINE struct step best_step(struct imm_viterbi *x, int row,
                                 int_fast16_t dst, bool tardy_state,
                                 bool safe_past)
{
  struct step step = step_init();
  int trans = 0;

  while (dst == x->curr_trans->dst)
  {
    struct state src = unwrap_state(x, x->curr_trans->src);

    if (tardy_state) src.min = (int_fast8_t)imm_max(src.min, 1);
    if (!safe_past) src.max = (int_fast8_t)imm_min(src.max, row);

    float score = IMM_LPROB_ZERO;
    int_fast8_t emissize = 0;
    int_fast8_t seqsize_idx = 0;

    imm_assume(src.max <= IMM_STATE_MAX_SEQSIZE);
    UNROLL(IMM_STATE_MAX_SEQSIZE + 1)
    for (int_fast8_t i = src.min; i <= src.max; ++i)
    {
      float v = get_matrix_cell_score(x, row, src.idx, i);
      if (score < v)
      {
        emissize = i;
        seqsize_idx = i - src.min;
        score = v;
      }
    }
    score += x->curr_trans->score;

    if (step.score < score)
    {
      step.src_idx = src.idx;
      step.src_trans = (int_fast16_t)trans;
      step.src_emissize = emissize;
      step.src_seqsize_idx = seqsize_idx;
      step.emis_score = score - x->curr_trans->score;
      step.trans_score = x->curr_trans->score;
      step.score = score;
    }

    ++trans;
    viterbi_ctrans_next(x);
  }

  return step;
}

IMM_PURE float emission_score(struct imm_viterbi const *x, struct state state,
                              int row, int len)
{
  int code = imm_eseq_get(x->task->seq, row, len, state.min);
  return imm_emis_score(&x->dp->emis, state.idx, code);
}

IMM_INLINE void set_state_score(struct imm_viterbi const *x, int row,
                                struct state dst, int remain,
                                struct step const *bt, bool safe_future)
{
  float score = bt->score;
  if (bt->src_idx != IMM_STATE_NULL_IDX)
    imm_trellis_push(&x->task->trellis, score, bt->src_idx, bt->src_emissize);
  else x->task->trellis.head++;

  if (!safe_future) dst.max = (int_fast8_t)imm_min(dst.max, remain);

  imm_assume(dst.max <= IMM_STATE_MAX_SEQSIZE);

  UNROLL(IMM_STATE_MAX_SEQSIZE + 1)
  for (int_fast8_t i = dst.min; i <= dst.max; ++i)
  {
    float emis_score = emission_score(x, dst, row, i);
    float total = score + emis_score;
    set_matrix_cell_score(x, imm_cell(row, dst.idx, i), total);
  }
}

IMM_CONST int remains(int seqsize, int row, bool safe_future)
{
  if (safe_future) return IMM_STATE_MAX_SEQSIZE;
  if (seqsize >= IMM_STATE_MAX_SEQSIZE + row) return IMM_STATE_MAX_SEQSIZE;
  return seqsize - row;
}

IMM_INLINE void on_normal_state(struct imm_viterbi *x, int row,
                                struct state dst, bool safe_future,
                                bool safe_past, bool tardy_state)
{
  int remain = remains(x->seqsize, row, safe_future);

  struct step step = best_step(x, row, dst.idx, tardy_state, safe_past);
  set_state_score(x, row, dst, remain, &step, safe_future);
}

IMM_INLINE void on_tardy_state(struct imm_viterbi *x, int row,
                               struct tardy_state const tardy, bool safe_future,
                               bool safe_past)
{
  viterbi_ctrans_seek(x, tardy.trans_start);
  struct state dst = unwrap_state(x, tardy.state_idx);
  on_normal_state(x, row, dst, safe_future, safe_past, true);
}

IMM_INLINE void on_row(struct imm_viterbi *x, int row, bool has_tardy_state,
                       bool safe_future, bool safe_past)
{
  if (has_tardy_state)
  // printf("    ...\n");
  {
    imm_trellis_seek(&x->task->trellis, row, x->tardy_state.state_idx);
    on_tardy_state(x, row, x->tardy_state, safe_future, safe_past);
  }

  imm_trellis_seek(&x->task->trellis, row, 0);
  viterbi_ctrans_rewind(x);

  for (int i = 0; i < nstates(x); ++i)
  {
    struct state dst = unwrap_state(x, i);
    if (row == 0 && start_state_idx(x) == dst.idx)
    {
      x->task->trellis.head++;
      continue;
    }
    on_normal_state(x, row, dst, safe_future, safe_past, false);
  }
}

IMM_INLINE void viterbi_generic(struct imm_viterbi *x, bool has_tardy_state)
{
  set_matrix_cell_score(x, imm_cell(0, (int_fast16_t)start_state_idx(x), 0), 0);
  imm_trellis_seek(&x->task->trellis, 0, start_state_idx(x));
  imm_trellis_push(&x->task->trellis, 0, IMM_STATE_NULL_IDX,
                   IMM_STATE_NULL_SEQSIZE);

  struct imm_dp_safety const *y = &x->safety;

  on_row(x, 0, has_tardy_state, false, false);

  for (int r = y->safe_future.start; r < y->safe_future.stop; ++r)
    on_row(x, r, has_tardy_state, true, false);

  for (int r = y->safe.start; r < y->safe.stop; ++r)
    on_row(x, r, has_tardy_state, true, true);

  for (int r = y->unsafe.start; r < y->unsafe.stop; ++r)
    on_row(x, r, has_tardy_state, false, false);

  for (int r = y->safe_past.start; r < y->safe_past.stop; ++r)
    on_row(x, r, has_tardy_state, false, true);
}

void imm_viterbi_run(struct imm_viterbi *x)
{
  imm_task_prepare(x->task);
  if (x->has_tardy_state) viterbi_generic(x, true);
  else viterbi_generic(x, false);
}
