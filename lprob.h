#ifndef IMM_LPROB_H
#define IMM_LPROB_H

#include "compiler.h"
#include "logaddexp.h"
#include <math.h>
#include <stdbool.h>
#include <stddef.h>

#define IMM_LPROB_NAN NAN
#define IMM_LPROB_ONE 0.0f
#define IMM_LPROB_ZERO -INFINITY

struct imm_rnd;

IMM_API float imm_lprob_add(float, float);
IMM_API float imm_lprob_nan(void);
IMM_API bool imm_lprob_is_nan(float);
IMM_API bool imm_lprob_is_zero(float);
IMM_API bool imm_lprob_is_finite(float);
IMM_API void imm_lprob_normalize(int size, float *arr);
IMM_API void imm_lprob_sample(struct imm_rnd *, int size, float *arr);
IMM_API float imm_lprob_zero(void);

IMM_PURE float imm_lprob_sum(int size, float const *arr)
{
  float r = arr[0];
  for (int i = 1; i < size; ++i)
    r = imm_logaddexp(r, arr[i]);
  return r;
}

#endif
