#ifndef IMM_MATH_H_API
#define IMM_MATH_H_API

#include "imm/api.h"

#define LOG0 -INFINITY

IMM_API int imm_isninf(double a);
IMM_API int imm_isnan(double a);
IMM_API double imm_logsumexp(double *arr, int len);
IMM_API int imm_lognormalize(double *arr, int len);

#endif
