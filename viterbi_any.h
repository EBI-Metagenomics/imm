#ifndef IMM_VITERBI_ANY_H
#define IMM_VITERBI_ANY_H

#include "compiler.h"
#include "remains.h"
#include "state_range.h"
#include "unroll.h"
#include "unsafe.h"
#include "viterbi.h"
#include "viterbi_best_incoming.h"

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

TEMPLATE void viterbi_set_scores_xy(struct imm_viterbi const *x,
                                    unsigned const row,
                                    struct state_range const dst,
                                    float const score)
{
  imm_assume(dst.max <= IMM_STATE_MAX_SEQLEN);

  UNROLL(IMM_STATE_MAX_SEQLEN + 1)
  for (uint_fast8_t i = dst.min; i <= dst.max; ++i)
  {
    float total = score + imm_viterbi_emission(x, row, dst.idx, i, dst.min);
    imm_viterbi_set_score(x, imm_cell(row, dst.idx, i), total);
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

TEMPLATE struct imm_ctrans const *
gany_state(struct imm_viterbi const *x, unsigned const row,
           struct state_range dst, struct imm_ctrans const *t,
           unsigned const seqlen, bool const save_path, bool const safe_future,
           bool const safe_past, bool const unsafe_state)
{
  float score = 0;
  struct viterbi_best_trans bt = {0};
  unsigned const remain = remains(seqlen, row, safe_future);

  t = viterbi_best_incoming(&score, &bt, x, row, dst.idx, t, save_path,
                            unsafe_state, safe_past);
  viterbi_set_state_score(x, row, dst, remain, score, &bt, save_path,
                          safe_future);
  return t;
}

TEMPLATE void gany_unsafe_state(struct imm_viterbi const *x, unsigned const row,
                                struct unsafe const unsafe,
                                unsigned const seqlen, bool const save_path,
                                bool const safe_future, bool const safe_past)
{
  struct imm_ctrans const *t = x->dp->trans_table.trans + unsafe.trans;
  struct state_range dst = imm_viterbi_state_range(x, unsafe.state);
  gany_state(x, row, dst, t, seqlen, save_path, safe_future, safe_past, true);
}

TEMPLATE struct imm_ctrans const *
gany_safe_states(struct imm_ctrans const *t, struct imm_range const states,
                 struct imm_viterbi const *x, unsigned const row,
                 unsigned const seqlen, bool const save_path,
                 bool const safe_future, bool const safe_past)
{
  for (unsigned i = states.a; i < states.b; ++i)
  {
    struct state_range dst = imm_viterbi_state_range(x, i);
    t = gany_state(x, row, dst, t, seqlen, save_path, safe_future, safe_past,
                   false);
  }
  return t;
}

TEMPLATE void gany(struct imm_viterbi const *x, unsigned const row,
                   struct unsafe const unsafe, unsigned const seqlen,
                   bool const save_path, bool const safe_future,
                   bool const safe_past)
{
  gany_unsafe_state(x, row, unsafe, seqlen, save_path, safe_future, safe_past);
  struct imm_ctrans const *t = x->dp->trans_table.trans;
  struct imm_range const r = imm_range(0, x->dp->state_table.nstates);
  gany_safe_states(t, r, x, row, seqlen, save_path, safe_future, safe_past);
}

#endif
