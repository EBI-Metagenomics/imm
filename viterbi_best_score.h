#ifndef IMM_VITERBI_BEST_SCORE_H
#define IMM_VITERBI_BEST_SCORE_H

#include "compiler.h"
#include "lprob.h"
#include "state_range.h"
#include "tuple.h"
#include "unroll.h"
#include "viterbi.h"

IMM_TEMPLATE void
viterbi_best_score_single(struct tuple_f32_uint8 *tuple,
                          struct imm_viterbi const *x, unsigned const row,
                          uint_fast16_t const src, uint_fast8_t const min,
                          uint_fast8_t const i, bool const save_path)
{
  imm_assume(row >= i);
  imm_assume(i >= min);
  float v = imm_viterbi_get_score(x, imm_cell(row - i, src, i));
  if (tuple->f < v && save_path) tuple->u = i - min;
  if (tuple->f < v) tuple->f = v;
}

IMM_PURE IMM_TEMPLATE struct tuple_f32_uint8
viterbi_best_score_xy(struct imm_viterbi const *x, unsigned const row,
                      struct state_range const src, bool const save_path)
{
  struct tuple_f32_uint8 tuple = {IMM_LPROB_ZERO, 0};

  imm_assume(src.max <= IMM_STATE_MAX_SEQLEN);
  UNROLL(IMM_STATE_MAX_SEQLEN + 1)
  for (uint_fast8_t i = src.min; i <= src.max; ++i)
    viterbi_best_score_single(&tuple, x, row, src.idx, src.min, i, save_path);

  return tuple;
}

IMM_PURE IMM_TEMPLATE struct tuple_f32_uint8
viterbi_best_score_15(struct imm_viterbi const *x, unsigned const row,
                      uint_fast16_t const src, bool const save_path)
{
  struct tuple_f32_uint8 tuple = {IMM_LPROB_ZERO, 0};

#pragma omp simd
  for (uint_fast8_t i = 1; i <= 5; ++i)
    viterbi_best_score_single(&tuple, x, row, src, 1, i, save_path);

  return tuple;
}

IMM_PURE IMM_TEMPLATE struct tuple_f32_uint8
viterbi_best_score_11(struct imm_viterbi const *x, unsigned const row,
                      uint_fast16_t const src, bool const save_path)
{
  struct tuple_f32_uint8 tuple = {IMM_LPROB_ZERO, 0};
  viterbi_best_score_single(&tuple, x, row, src, 1, 1, save_path);
  return tuple;
}

IMM_PURE IMM_TEMPLATE struct tuple_f32_uint8
viterbi_best_score_00(struct imm_viterbi const *x, unsigned const row,
                      uint_fast16_t const src, bool const save_path)
{
  struct tuple_f32_uint8 tuple = {IMM_LPROB_ZERO, 0};
  viterbi_best_score_single(&tuple, x, row, src, 0, 0, save_path);
  return tuple;
}

#endif
