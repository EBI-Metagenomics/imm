#ifndef IMM_FLOAT_H
#define IMM_FLOAT_H

#include "imm/config.h"
#include <tgmath.h>

#ifdef I
#undef I
#endif

#ifdef IMM_DOUBLE_PRECISION
typedef double imm_float;
#else
typedef float imm_float;
#endif

#define imm_log(x) ((imm_float)log(x))

#define IMM_ARR(...)                                                           \
    (imm_float[]) { __VA_ARGS__ }

#endif
