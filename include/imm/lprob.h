#ifndef IMM_LPROB_H_API
#define IMM_LPROB_H_API

#include "imm/api.h"

IMM_API double imm_lprob_impossible(void);
IMM_API int    imm_lprob_is_possible(double a);
IMM_API int    imm_lprob_is_valid(double a);
IMM_API double imm_lprob_invalid(void);
IMM_API double imm_lprob_add(double a, double b);
IMM_API double imm_lprob_sum(double* arr, int len);
IMM_API int    imm_lprob_normalize(double* arr, int len);

#endif
