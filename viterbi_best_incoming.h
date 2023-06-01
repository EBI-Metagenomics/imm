#ifndef IMM_VITERBI_BEST_INCOMING_H
#define IMM_VITERBI_BEST_INCOMING_H

#include "compiler.h"
#include "lprob.h"
#include "max.h"
#include "min.h"
#include "state_range.h"
#include "unroll.h"
#include "viterbi.h"

struct viterbi_best_trans
{
  uint_fast16_t src_idx;
  uint_fast16_t src_trans;
  uint_fast8_t src_seqlen;
};

struct tuple_f32_uint8
{
  float f;
  uint_fast8_t u;
};

#define VITERBI_BEST_TRANS_INIT                                                \
  {                                                                            \
    IMM_STATE_NULL_IDX, IMM_TRANS_NULL_IDX, IMM_STATE_NULL_SEQLEN              \
  }

#define VITERBI_BEST_TRANS_DECLARE(NAME)                                       \
  struct viterbi_best_trans NAME = VITERBI_BEST_TRANS_INIT

TEMPLATE void viterbi_best_trans_set(struct viterbi_best_trans *x,
                                     uint_fast16_t const src,
                                     uint_fast16_t const trans,
                                     uint_fast8_t const len)
{
  x->src_idx = src;
  x->src_trans = trans;
  x->src_seqlen = len;
}

PURE_ATTR TEMPLATE struct tuple_f32_uint8
viterbi_best_score_xy(struct imm_viterbi const *x, unsigned const row,
                      struct state_range const src)
{
  struct tuple_f32_uint8 tuple = {IMM_LPROB_ZERO, 0};

  imm_assume(src.max <= IMM_STATE_MAX_SEQLEN);
  UNROLL(IMM_STATE_MAX_SEQLEN + 1)
  for (uint_fast8_t i = src.min; i <= src.max; ++i)
  {
    imm_assume(row >= i);
    imm_assume(i >= src.min);
    float v = imm_viterbi_get_score(x, imm_cell(row - i, src.idx, i));
    if (tuple.f < v)
    {
      tuple.u = i - src.min;
      tuple.f = v;
    }
  }

  return tuple;
}

TEMPLATE void
viterbi_best_incoming_get(float *score, struct viterbi_best_trans *bt,
                          struct imm_viterbi const *x, unsigned const row,
                          struct state_range const src,
                          uint_fast16_t const trans, float const trans_score)
{
  struct tuple_f32_uint8 t = viterbi_best_score_xy(x, row, src);
  t.f += trans_score;

  if (*score < t.f)
  {
    viterbi_best_trans_set(bt, src.idx, trans, t.u);
    *score = t.f;
  }
}

TEMPLATE struct imm_ctrans const *
viterbi_best_incoming(float *score, struct viterbi_best_trans *best_trans,
                      struct imm_viterbi const *x, unsigned const row,
                      uint_fast16_t const dst, struct imm_ctrans const *ctrans,
                      bool const unsafe_state, bool const safe_past)
{
  float v = IMM_LPROB_ZERO;
  VITERBI_BEST_TRANS_DECLARE(w);
  unsigned trans = 0;

  while (dst == ctrans->dst)
  {
    struct state_range src = imm_viterbi_state_range(x, ctrans->src);
    float const tscore = ctrans->score;

    if (unsafe_state) src.min = imm_max(src.min, 1U);
    if (!safe_past) src.max = imm_min(src.max, row);

    viterbi_best_incoming_get(&v, &w, x, row, src, trans, tscore);

    ++trans;
    ++ctrans;
  }

  *score = v;
  *best_trans = w;
  return ctrans;
}

#endif
