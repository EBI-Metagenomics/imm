#ifndef MATRIXU_H
#define MATRIXU_H

#include "make_matrix.h"

MAKE_MATRIX_STRUCT(matrixu, unsigned)
MAKE_MATRIX_CREATE(matrixu, unsigned)
MAKE_MATRIX_GET(matrixu, unsigned)
MAKE_MATRIX_GET_PTR(matrixu, unsigned)
MAKE_MATRIX_GET_PTR_C(matrixu, unsigned)
MAKE_MATRIX_SET(matrixu, unsigned)
MAKE_MATRIX_DESTROY(matrixu)
MAKE_MATRIX_NROWS(matrixu)
MAKE_MATRIX_NCOLS(matrixu)
MAKE_MATRIX_RESIZE(matrixu, unsigned)

#endif
