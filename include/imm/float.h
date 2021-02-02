#ifndef IMM_FLOAT_H
#define IMM_FLOAT_H

#include "imm/config.h"

#ifdef IMM_DOUBLE_PRECISION
typedef double imm_float;
#else
typedef float imm_float;
#endif

#endif
