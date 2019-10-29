#ifndef IMM_LPROB_H
#define IMM_LPROB_H

#include "imm/api.h"

IMM_API double imm_lprob_zero(void);
IMM_API int    imm_lprob_is_zero(double a);
IMM_API int    imm_lprob_is_valid(double a);
IMM_API double imm_lprob_invalid(void);
IMM_API double imm_lprob_add(double a, double b);
IMM_API double imm_lprob_sum(double* arr, int len);
IMM_API int    imm_lprob_normalize(double* arr, int len);

#endif
