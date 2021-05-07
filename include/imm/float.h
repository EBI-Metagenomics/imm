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

#define IMM_NARGS(...) (sizeof((int[]){__VA_ARGS__}) / sizeof(int))

#define IMM_FARR(...)                                                          \
    (imm_float[IMM_NARGS(__VA_ARGS__)]) { __VA_ARGS__ }

#endif
