#ifndef IMM_VITERBI_SET_STATE_SCORE_H
#define IMM_VITERBI_SET_STATE_SCORE_H

#include "compiler.h"
#include "minmax.h"
#include "viterbi.h"
#include "viterbi_best_trans.h"
#include "viterbi_set_scores.h"

TEMPLATE void viterbi_set_state_score_path(struct imm_cpath *x,
                                           struct viterbi_best_trans const *bt,
                                           unsigned const r,
                                           uint_fast16_t const dst)
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

TEMPLATE void
viterbi_set_state_score(struct imm_viterbi const *x, unsigned const row,
                        struct state_range dst, unsigned const remain,
                        float score, struct viterbi_best_trans const *bt,
                        bool const save_path, bool const safe_future)
{
  if (row == 0 && imm_viterbi_start_state(x) == dst.idx)
    score = MAX(imm_viterbi_start_lprob(x), score);

  if (save_path) viterbi_set_state_score_path(&x->task->path, bt, row, dst.idx);
  if (!safe_future) dst.max = MIN(dst.max, remain);

  viterbi_set_scores_xy(x, row, dst, score);
}

TEMPLATE void
viterbi_set_state_score_15_fast(struct imm_viterbi const *x, unsigned const row,
                                uint_fast16_t const dst, float const score,
                                struct viterbi_best_trans const *bt,
                                bool const save_path)
{
  if (save_path) viterbi_set_state_score_path(&x->task->path, bt, row, dst);
  viterbi_set_scores_15(x, row, dst, score);
}

TEMPLATE void
viterbi_set_state_score_11_fast(struct imm_viterbi const *x, unsigned const row,
                                uint_fast16_t const dst, float const score,
                                struct viterbi_best_trans const *bt,
                                bool const save_path)
{
  if (save_path) viterbi_set_state_score_path(&x->task->path, bt, row, dst);
  viterbi_set_scores_11(x, row, dst, score);
}

TEMPLATE void
viterbi_set_state_score_00_fast(struct imm_viterbi const *x, unsigned const row,
                                uint_fast16_t const dst, float const score,
                                struct viterbi_best_trans const *bt,
                                bool const save_path)
{
  if (save_path) viterbi_set_state_score_path(&x->task->path, bt, row, dst);
  viterbi_set_scores_00(x, row, dst, score);
}

#endif