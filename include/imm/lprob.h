#ifndef IMM_LPROB_H
#define IMM_LPROB_H

#include "imm/api.h"
#include "logaddexp.h"
#include <math.h>
#include <stdbool.h>
#include <stddef.h>

IMM_API static inline double imm_lprob_zero(void) { return -INFINITY; }
IMM_API static inline bool   imm_lprob_is_zero(double a) { return isinf(a) && a < 0.0; }
IMM_API static inline bool   imm_lprob_is_valid(double a) { return !isnan(a); }
IMM_API static inline double imm_lprob_invalid(void) { return NAN; }
IMM_API static inline double imm_lprob_add(double a, double b) { return logaddexp(a, b); }
IMM_API double               imm_lprob_sum(double const* arr, size_t len);
IMM_API int                  imm_lprob_normalize(double* arr, size_t len);

#endif
