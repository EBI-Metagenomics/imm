#ifndef MATRIX_MATRIX_H
#define MATRIX_MATRIX_H

#include "make_matrix.h"
#include <stdint.h>

#define MAKE_MATRIX(name, type)                                                \
    MAKE_MATRIX_STRUCT(name, type)                                             \
    MAKE_MATRIX_INIT(name, type)                                               \
    MAKE_MATRIX_EMPTY(name, type)                                              \
    MAKE_MATRIX_GET(name, type)                                                \
    MAKE_MATRIX_GET_PTR(name, type)                                            \
    MAKE_MATRIX_GET_PTR_C(name, type)                                          \
    MAKE_MATRIX_SET(name, type)                                                \
    MAKE_MATRIX_DEINIT(name)                                                   \
    MAKE_MATRIX_NROWS(name)                                                    \
    MAKE_MATRIX_NCOLS(name)                                                    \
    MAKE_MATRIX_RESIZE(name, type)

MAKE_MATRIX(matrixu16, uint16_t)

#endif
