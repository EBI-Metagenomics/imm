#include "lprob.h"
#include "logaddexp.h"
#include "rnd.h"

float imm_lprob_add(float a, float b) { return imm_logaddexp(a, b); }

float imm_lprob_nan(void) { return NAN; }

bool imm_lprob_is_nan(float a) { return isnan(a); }

bool imm_lprob_is_zero(float a) { return isinf(a) && a < 0.0; }

bool imm_lprob_is_finite(float a)
{
  return !imm_lprob_is_nan(a) && !imm_lprob_is_zero(a);
}

void imm_lprob_normalize(int size, float *arr)
{
  float lnorm = imm_lprob_sum(size, arr);
  for (int i = 0; i < size; ++i)
    arr[i] -= lnorm;
}

void imm_lprob_sample(struct imm_rnd *rnd, int size, float *lprobs)
{
  for (int i = 0; i < size; ++i)
    lprobs[i] = (float)log(imm_rnd_dbl(rnd));
}

float imm_lprob_zero(void) { return -INFINITY; }
