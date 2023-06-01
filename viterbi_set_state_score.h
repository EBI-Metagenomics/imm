#ifndef IMM_VITERBI_SET_STATE_SCORE_H
#define IMM_VITERBI_SET_STATE_SCORE_H

#include "compiler.h"
#include "max.h"
#include "min.h"
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
    score = imm_max(imm_viterbi_start_lprob(x), score);

  if (save_path) viterbi_set_state_score_path(&x->task->path, bt, row, dst.idx);
  if (!safe_future) dst.max = imm_min(dst.max, remain);

  viterbi_set_scores_xy(x, row, dst, score);
}

#endif
