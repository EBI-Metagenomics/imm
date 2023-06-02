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

TEMPLATE struct imm_ctrans const *
viterbi_best_incoming(float *score, struct viterbi_best_trans *best_trans,
                      struct imm_viterbi const *x, unsigned const row,
                      uint_fast16_t const dst, struct imm_ctrans const *ctrans,
                      bool const unsafe_state, bool const safe_past)
{
  float local_score = IMM_LPROB_ZERO;
  VITERBI_BEST_TRANS_DECLARE(w);
  unsigned trans = 0;

  while (dst == ctrans->dst)
  {
    struct state_range src = imm_viterbi_state_range(x, ctrans->src);
    float const tscore = ctrans->score;

    if (unsafe_state) src.min = imm_max(src.min, 1U);
    if (!safe_past) src.max = imm_min(src.max, row);

    struct tuple_f32_uint8 t = {IMM_LPROB_ZERO, 0};

    imm_assume(src.max <= IMM_STATE_MAX_SEQLEN);
    UNROLL(IMM_STATE_MAX_SEQLEN + 1)
    for (uint_fast8_t i = src.min; i <= src.max; ++i)
    {
      imm_assume(row >= i);
      imm_assume(i >= src.min);
      float v = imm_viterbi_get_score(x, imm_cell(row - i, src.idx, i));
      if (t.f < v)
      {
        t.u = i - src.min;
        t.f = v;
      }
    }
    t.f += tscore;

    if (local_score < t.f)
    {
      w.src_idx = src.idx;
      w.src_trans = trans;
      w.src_seqlen = t.u;
      local_score = t.f;
    }

    ++trans;
    ++ctrans;
  }

  *score = local_score;
  *best_trans = w;
  return ctrans;
}

#endif
