#ifndef IMM_VITERBI_BEST_INCOMING_GET_H
#define IMM_VITERBI_BEST_INCOMING_GET_H

#include "state_range.h"
#include "viterbi.h"
#include "viterbi_best_score.h"
#include "viterbi_best_trans.h"

TEMPLATE void viterbi_best_incoming_get_update(
    float const v, uint_fast8_t const w, float *score,
    struct viterbi_best_trans *bt, uint_fast16_t const src,
    uint_fast16_t const trans, bool const save_path)
{
  if (*score < v && save_path) viterbi_best_trans_set(bt, src, trans, w);
  if (*score < v) *score = v;
}

TEMPLATE void viterbi_best_incoming_get(
    float *score, struct viterbi_best_trans *bt, struct imm_viterbi const *x,
    unsigned const row, struct state_range const src, uint_fast16_t const trans,
    float const trans_score, bool const save_path)
{
  struct tuple_f32_uint8 t = viterbi_best_score_xy(x, row, src, save_path);
  t.f += trans_score;

  viterbi_best_incoming_get_update(t.f, t.u, score, bt, src.idx, trans,
                                   save_path);
}

#endif
