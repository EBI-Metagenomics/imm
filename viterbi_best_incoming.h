#ifndef IMM_VITERBI_BEST_INCOMING_H
#define IMM_VITERBI_BEST_INCOMING_H

#include "compiler.h"
#include "lprob.h"
#include "max.h"
#include "min.h"
#include "state_range.h"
#include "unroll.h"
#include "viterbi.h"
#include "viterbi_best_incoming_get.h"
#include "viterbi_best_score.h"
#include "viterbi_best_trans.h"

TEMPLATE void viterbi_best_incoming_fix_range(struct state_range *x,
                                              unsigned const row,
                                              bool const unsafe_state,
                                              bool const safe_past)
{
  if (unsafe_state) x->min = imm_max(x->min, 1U);
  if (!safe_past) x->max = imm_min(x->max, row);
}

TEMPLATE struct imm_ctrans const *
viterbi_best_incoming(float *score, struct viterbi_best_trans *best_trans,
                      struct imm_viterbi const *x, unsigned const row,
                      uint_fast16_t const dst, struct imm_ctrans const *ctrans,
                      bool const save_path, bool const unsafe_state,
                      bool const safe_past)
{
  float v = IMM_LPROB_ZERO;
  VITERBI_BEST_TRANS_DECLARE(w);
  unsigned trans = 0;

  while (dst == ctrans->dst)
  {
    struct state_range src = imm_viterbi_state_range(x, ctrans->src);
    float const tscore = ctrans->score;

    viterbi_best_incoming_fix_range(&src, row, unsafe_state, safe_past);
    viterbi_best_incoming_get(&v, &w, x, row, src, trans, tscore, save_path);

    ++trans;
    ++ctrans;
  }

  *score = v;
  *best_trans = w;
  return ctrans;
}

#endif
