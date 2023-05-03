#ifndef IMM_VITERBI_SET_SCORES_H
#define IMM_VITERBI_SET_SCORES_H

#include "compiler.h"
#include "state_range.h"
#include "unroll.h"
#include "viterbi.h"

TEMPLATE void viterbi_set_scores_single(struct imm_viterbi const *x,
                                        unsigned const row,
                                        uint_fast16_t const dst,
                                        uint_fast8_t const min,
                                        uint_fast8_t const i, float const score)
{
  float total = score + imm_viterbi_emission(x, row, dst, i, min);
  imm_viterbi_set_score(x, imm_cell(row, dst, i), total);
}

TEMPLATE void viterbi_set_scores_xy(struct imm_viterbi const *x,
                                    unsigned const row,
                                    struct state_range const dst,
                                    float const score)
{
  imm_assume(dst.max <= IMM_STATE_MAX_SEQLEN);

  UNROLL(IMM_STATE_MAX_SEQLEN + 1)
  for (uint_fast8_t i = dst.min; i <= dst.max; ++i)
    viterbi_set_scores_single(x, row, dst.idx, dst.min, i, score);
}

TEMPLATE void viterbi_set_scores_15(struct imm_viterbi const *x,
                                    unsigned const row, uint_fast16_t const dst,
                                    float const score)
{
#pragma omp simd
  for (uint_fast8_t i = 1; i <= 5; ++i)
    viterbi_set_scores_single(x, row, dst, 1, i, score);
}

TEMPLATE void viterbi_set_scores_11(struct imm_viterbi const *x,
                                    unsigned const row, uint_fast16_t const dst,
                                    float const score)
{
  viterbi_set_scores_single(x, row, dst, 1, 1, score);
}

TEMPLATE void viterbi_set_scores_00(struct imm_viterbi const *x,
                                    unsigned const row, uint_fast16_t const dst,
                                    float const score)
{
  viterbi_set_scores_single(x, row, dst, 0, 0, score);
}

#endif
