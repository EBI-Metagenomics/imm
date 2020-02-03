#ifndef IMM_LPROB_H
#define IMM_LPROB_H

#include "imm/api.h"
#include <math.h>
#include <stdbool.h>
#include <stddef.h>

#define IMM_LPROB_ZERO -INFINITY
#define IMM_LPROB_INVALID NAN

IMM_API double imm_lprob_zero(void);
IMM_API bool   imm_lprob_is_zero(double a);
IMM_API bool   imm_lprob_is_valid(double a);
IMM_API double imm_lprob_invalid(void);
IMM_API double imm_lprob_add(double a, double b);
IMM_API double imm_lprob_sum(double const* arr, size_t len);
IMM_API int    imm_lprob_normalize(double* arr, size_t len);

#endif
