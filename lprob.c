#include "lprob.h"
#include "rnd.h"
#include "vendor/logaddexp.h"

float imm_lprob_add(float a, float b) { return logaddexp(a, b); }

float imm_lprob_nan(void) { return NAN; }

bool imm_lprob_is_nan(float a) { return isnan(a); }

bool imm_lprob_is_zero(float a) { return isinf(a) && a < 0.0; }

bool imm_lprob_is_finite(float a)
{
  return !imm_lprob_is_nan(a) && !imm_lprob_is_zero(a);
}

void imm_lprob_normalize(unsigned len, float *arr)
{
  float lnorm = imm_lprob_sum(len, arr);
#pragma omp simd
  for (unsigned i = 0; i < len; ++i)
    arr[i] -= lnorm;
}

void imm_lprob_sample(struct imm_rnd *rnd, unsigned len, float *lprobs)
{
  for (unsigned i = 0; i < len; ++i)
    lprobs[i] = log(imm_rnd_dbl(rnd));
}

float imm_lprob_zero(void) { return -INFINITY; }
