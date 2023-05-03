#ifndef IMM_VITERBI_BEST_INCOMING_H
#define IMM_VITERBI_BEST_INCOMING_H

#include "compiler.h"
#include "lprob.h"
#include "minmax.h"
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
  if (unsafe_state) x->min = MAX(x->min, 1U);
  if (!safe_past) x->max = MIN(x->max, row);
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

TEMPLATE struct imm_ctrans const *viterbi_best_incoming_0015(
    float *score, struct viterbi_best_trans *best_trans,
    struct imm_viterbi const *x, unsigned const row, uint_fast16_t const src0,
    uint_fast16_t const src1, struct imm_ctrans const *ctrans,
    bool const save_path, bool const unsafe_state, bool const safe_past)
{
  float v = IMM_LPROB_ZERO;
  VITERBI_BEST_TRANS_DECLARE(w);

  struct state_range src[2] = {STATE_RANGE(src0, 0, 0),
                               STATE_RANGE(src1, 1, 5)};
  float const tscore[2] = {ctrans[0].score, ctrans[1].score};

  viterbi_best_incoming_fix_range(&src[0], row, unsafe_state, safe_past);
  viterbi_best_incoming_get(&v, &w, x, row, src[0], 0, tscore[0], save_path);

  viterbi_best_incoming_fix_range(&src[1], row, unsafe_state, safe_past);
  viterbi_best_incoming_get(&v, &w, x, row, src[1], 1, tscore[1], save_path);

  *score = v;
  *best_trans = w;
  return ctrans + 2;
}

TEMPLATE struct imm_ctrans const *viterbi_best_incoming_0015_fast(
    float *score, struct viterbi_best_trans *best_trans,
    struct imm_viterbi const *x, unsigned const row, uint_fast16_t const src0,
    uint_fast16_t const src1, struct imm_ctrans const *ctrans,
    bool const save_path)
{
  float v = IMM_LPROB_ZERO;
  VITERBI_BEST_TRANS_DECLARE(w);

  float const tscore[2] = {ctrans[0].score, ctrans[1].score};

  viterbi_best_incoming_get_00(&v, &w, x, row, src0, 0, tscore[0], save_path);
  viterbi_best_incoming_get_15(&v, &w, x, row, src1, 1, tscore[1], save_path);

  *score = v;
  *best_trans = w;
  return ctrans + 2;
}

TEMPLATE struct imm_ctrans const *viterbi_best_incoming_1100(
    float *score, struct viterbi_best_trans *best_trans,
    struct imm_viterbi const *x, unsigned const row, uint_fast16_t const src0,
    uint_fast16_t const src1, struct imm_ctrans const *ctrans,
    bool const save_path, bool const unsafe_state, bool const safe_past)
{
  float v = IMM_LPROB_ZERO;
  VITERBI_BEST_TRANS_DECLARE(w);

  struct state_range src[2] = {STATE_RANGE(src0, 1, 1),
                               STATE_RANGE(src1, 0, 0)};
  float const tscore[2] = {ctrans[0].score, ctrans[1].score};

  viterbi_best_incoming_fix_range(&src[0], row, unsafe_state, safe_past);
  viterbi_best_incoming_get(&v, &w, x, row, src[0], 0, tscore[0], save_path);

  viterbi_best_incoming_fix_range(&src[1], row, unsafe_state, safe_past);
  viterbi_best_incoming_get(&v, &w, x, row, src[1], 1, tscore[1], save_path);

  *score = v;
  *best_trans = w;
  return ctrans + 2;
}

TEMPLATE struct imm_ctrans const *viterbi_best_incoming_1100_fast(
    float *score, struct viterbi_best_trans *best_trans,
    struct imm_viterbi const *x, unsigned const row, uint_fast16_t const src0,
    uint_fast16_t const src1, struct imm_ctrans const *ctrans,
    bool const save_path)
{
  float v = IMM_LPROB_ZERO;
  VITERBI_BEST_TRANS_DECLARE(w);

  float const tscore[2] = {ctrans[0].score, ctrans[1].score};

  viterbi_best_incoming_get_11(&v, &w, x, row, src0, 0, tscore[0], save_path);
  viterbi_best_incoming_get_00(&v, &w, x, row, src1, 1, tscore[1], save_path);

  *score = v;
  *best_trans = w;
  return ctrans + 2;
}

#endif
