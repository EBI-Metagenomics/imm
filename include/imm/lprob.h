#ifndef IMM_LPROB_H
#define IMM_LPROB_H

#include "imm/export.h"
#include "imm/float.h"
#include "logaddexp/logaddexp.h"
#include <stdbool.h>
#include <stddef.h>

#define IMM_LPROB_NAN NAN
#define IMM_LPROB_ONE 0.0f
#define IMM_LPROB_ZERO -INFINITY

static inline imm_float imm_lprob_add(imm_float a, imm_float b)
{
    return logaddexp(a, b);
}

static inline imm_float imm_lprob_nan(void) { return IMM_LPROB_NAN; }

static inline bool imm_lprob_is_nan(imm_float a) { return isnan(a); }

static inline bool imm_lprob_is_zero(imm_float a)
{
    return isinf(a) && a < 0.0;
}

static inline bool imm_lprob_is_finite(imm_float a)
{
    return !imm_lprob_is_nan(a) && !imm_lprob_is_zero(a);
}

IMM_API int imm_lprob_normalize(unsigned len, imm_float arr[len]);

static inline imm_float imm_lprob_sum(unsigned len, imm_float const arr[len])
{
    imm_float r = arr[0];
    for (unsigned i = 1; i < len; ++i)
        r = logaddexp(r, arr[i]);
    return r;
}

static inline imm_float imm_lprob_zero(void) { return IMM_LPROB_ZERO; }

#endif
