#ifndef IMM_LOG_H_API
#define IMM_LOG_H_API

#include "imm/api.h"

IMM_API double imm_log_sumexp(double *arr, int len);
IMM_API int imm_log_normalize(double *arr, int len);

#endif
