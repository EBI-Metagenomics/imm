#ifndef IMM_LPROB_H
#define IMM_LPROB_H

#include "imm/report.h"
#include "logaddexp/logaddexp.h"
#include <math.h>
#include <stdbool.h>
#include <stddef.h>

static inline double imm_lprob_add(double a, double b) { return logaddexp(a, b); }
static inline double imm_lprob_invalid(void) { return NAN; }
static inline bool   imm_lprob_is_valid(double a) { return !isnan(a); }
static inline bool   imm_lprob_is_zero(double a) { return isinf(a) && a < 0.0; }
static inline int    imm_lprob_normalize(double* arr, size_t len);
static inline double imm_lprob_sum(double const* arr, size_t len);
static inline double imm_lprob_zero(void) { return -INFINITY; }

static inline int imm_lprob_normalize(double* arr, size_t len)
{
    double lnorm = imm_lprob_sum(arr, len);
    if (!isfinite(lnorm)) {
        imm_error("non-finite normalization denominator");
        return 1;
    }

    for (size_t i = 0; i < len; ++i)
        arr[i] -= lnorm;

    return 0;
}

static inline double imm_lprob_sum(double const* arr, size_t len)
{
    double r = imm_lprob_zero();
    for (size_t i = 0; i < len; ++i)
        r = logaddexp(r, arr[i]);
    return r;
}

#endif
