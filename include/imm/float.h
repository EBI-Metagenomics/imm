#ifndef IMM_FLOAT_H
#define IMM_FLOAT_H

#include "imm/config.h"
#include <float.h>
#include <math.h>

#ifdef IMM_DOUBLE_PRECISION
typedef double imm_float;
#define IMM_FLOAT_BYTES 8
#define IMM_FLOAT_MAX DBL_MAX
#else
typedef float imm_float;
#define IMM_FLOAT_BYTES 4
#define IMM_FLOAT_MAX FLT_MAX
#endif

#define imm_log(x) ((imm_float)log(x))
#define imm_exp(x) ((imm_float)exp(x))

#define IMM_ARR(...)                                                           \
    (imm_float[]) { __VA_ARGS__ }

#endif
