#ifndef MATRIX_DOUBLE_H
#define MATRIX_DOUBLE_H

#include "matrix.h"

/* Defines `struct matrix_double` type. */
MAKE_MATRIX_STRUCT(matrix_double, double)
MAKE_MATRIX_CREATE(matrix_double, double)
MAKE_MATRIX_GET(matrix_double, double)
MAKE_MATRIX_GET_C(matrix_double, double)
MAKE_MATRIX_DESTROY(matrix_double)
MAKE_MATRIX_NROWS(matrix_double)
MAKE_MATRIX_NCOLS(matrix_double)
MAKE_MATRIX_RESIZE(matrix_double, double)

#endif
