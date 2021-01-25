#ifndef IMM_LPROB_H
#define IMM_LPROB_H

#include "imm/float.h"
#include "imm/report.h"
#include "logaddexp/logaddexp.h"
#include <math.h>
#include <stdbool.h>
#include <stddef.h>

static inline imm_float imm_lprob_add(imm_float a, imm_float b) { return logaddexp(a, b); }
static inline imm_float imm_lprob_invalid(void) { return NAN; }
static inline bool      imm_lprob_is_valid(imm_float a) { return !isnan(a); }
static inline bool      imm_lprob_is_zero(imm_float a) { return isinf(a) && a < 0.0; }
static inline int       imm_lprob_normalize(imm_float* arr, size_t len);
static inline imm_float imm_lprob_sum(imm_float const* arr, size_t len);
static inline imm_float imm_lprob_zero(void) { return -INFINITY; }

static inline int imm_lprob_normalize(imm_float* arr, size_t len)
{
    imm_float lnorm = imm_lprob_sum(arr, len);
    if (!isfinite(lnorm)) {
        imm_error("non-finite normalization denominator");
        return 1;
    }

    for (size_t i = 0; i < len; ++i)
        arr[i] -= lnorm;

    return 0;
}

static inline imm_float imm_lprob_sum(imm_float const* arr, size_t len)
{
    imm_float r = imm_lprob_zero();
    for (size_t i = 0; i < len; ++i)
        r = logaddexp(r, arr[i]);
    return r;
}

#endif
