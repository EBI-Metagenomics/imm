#ifndef IMM_LPROB_H
#define IMM_LPROB_H

#include "imm/export.h"
#include "imm/float.h"
#include "logaddexp/logaddexp.h"
#include <stdbool.h>
#include <stddef.h>

static inline imm_float imm_lprob_add(imm_float a, imm_float b) { return logaddexp(a, b); }
static inline imm_float imm_lprob_invalid(void) { return NAN; }
static inline bool      imm_lprob_is_valid(imm_float a) { return !isnan(a); }
static inline bool      imm_lprob_is_zero(imm_float a) { return isinf(a) && a < 0.0; }
static inline bool      imm_lprob_is_finite(imm_float a) { return imm_lprob_is_valid(a) && !imm_lprob_is_zero(a); }
IMM_API int             imm_lprob_normalize(imm_float* arr, size_t len);
IMM_API imm_float       imm_lprob_sum(imm_float const* arr, size_t len);
static inline imm_float imm_lprob_zero(void) { return -INFINITY; }

#endif
