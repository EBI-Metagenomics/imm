#ifndef MATRIXF_H
#define MATRIXF_H

#include "make_matrix.h"

/* Defines `struct matrixf` type. */
MAKE_MATRIX_STRUCT(matrixf, float)
MAKE_MATRIX_CREATE(matrixf, float)
MAKE_MATRIX_GET(matrixf, float)
MAKE_MATRIX_GET_PTR(matrixf, float)
MAKE_MATRIX_GET_PTR_C(matrixf, float)
MAKE_MATRIX_SET(matrixf, float)
MAKE_MATRIX_DESTROY(matrixf)
MAKE_MATRIX_NROWS(matrixf)
MAKE_MATRIX_NCOLS(matrixf)
MAKE_MATRIX_RESIZE(matrixf, float)

#endif
