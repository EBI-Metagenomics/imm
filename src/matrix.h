#ifndef MATRIX_H
#define MATRIX_H

#include "imm/imm.h"
#include "make_matrix.h"

#define MAKE_MATRIX(name, type)                                                \
    MAKE_MATRIX_STRUCT(name, type)                                             \
    MAKE_MATRIX_CREATE(name, type)                                             \
    MAKE_MATRIX_GET(name, type)                                                \
    MAKE_MATRIX_GET_PTR(name, type)                                            \
    MAKE_MATRIX_GET_PTR_C(name, type)                                          \
    MAKE_MATRIX_SET(name, type)                                                \
    MAKE_MATRIX_DESTROY(name)                                                  \
    MAKE_MATRIX_NROWS(name)                                                    \
    MAKE_MATRIX_NCOLS(name)                                                    \
    MAKE_MATRIX_RESIZE(name, type)

MAKE_MATRIX(matrixu32, uint32_t)
MAKE_MATRIX(matrixu16, uint16_t)
MAKE_MATRIX(matrixu8, uint8_t)

MAKE_MATRIX(matrixf, imm_float)

#endif
