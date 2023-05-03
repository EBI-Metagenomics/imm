#ifndef IMM_VITERBI_BEST_SCORE_H
#define IMM_VITERBI_BEST_SCORE_H

#include "compiler.h"
#include "lprob.h"
#include "state_range.h"
#include "unroll.h"
#include "viterbi.h"

TEMPLATE void
viterbi_best_score_single(float *score, uint_fast8_t *length,
                          struct imm_viterbi const *x, unsigned const row,
                          uint_fast16_t const src, uint_fast8_t const min,
                          uint_fast8_t const i, bool const save_path)
{
  imm_assume(row >= i);
  imm_assume(i >= min);
  float v = imm_viterbi_get_score(x, imm_cell(row - i, src, i));
  if (*score < v && save_path) *length = i - min;
  if (*score < v) *score = v;
}

TEMPLATE void viterbi_best_score_xy(float *score, uint_fast8_t *length,
                                    struct imm_viterbi const *x,
                                    unsigned const row,
                                    struct state_range const src,
                                    bool const save_path)
{
  float v = IMM_LPROB_ZERO;
  uint_fast8_t w = 0;

  imm_assume(src.max <= IMM_STATE_MAX_SEQLEN);
  UNROLL(IMM_STATE_MAX_SEQLEN + 1)
  for (uint_fast8_t i = src.min; i <= src.max; ++i)
    viterbi_best_score_single(&v, &w, x, row, src.idx, src.min, i, save_path);

  *score = v;
  *length = w;
}

TEMPLATE void viterbi_best_score_15(float *score, uint_fast8_t *length,
                                    struct imm_viterbi const *x,
                                    unsigned const row, uint_fast16_t const src,
                                    bool const save_path)
{
  float v = IMM_LPROB_ZERO;
  uint_fast8_t w = 0;

#pragma omp simd
  for (uint_fast8_t i = 1; i <= 5; ++i)
    viterbi_best_score_single(&v, &w, x, row, src, 1, i, save_path);

  *score = v;
  *length = w;
}

TEMPLATE void viterbi_best_score_11(float *score, uint_fast8_t *length,
                                    struct imm_viterbi const *x,
                                    unsigned const row, uint_fast16_t const src,
                                    bool const save_path)
{
  float v = IMM_LPROB_ZERO;
  uint_fast8_t w = 0;

  viterbi_best_score_single(&v, &w, x, row, src, 1, 1, save_path);

  *score = v;
  *length = w;
}

TEMPLATE void viterbi_best_score_00(float *score, uint_fast8_t *length,
                                    struct imm_viterbi const *x,
                                    unsigned const row, uint_fast16_t const src,
                                    bool const save_path)
{
  float v = IMM_LPROB_ZERO;
  uint_fast8_t w = 0;

  viterbi_best_score_single(&v, &w, x, row, src, 0, 0, save_path);

  *score = v;
  *length = w;
}

#endif
