#ifndef MATRIXD_H
#define MATRIXD_H

#include "make_matrix.h"

MAKE_MATRIX_STRUCT(matrixd, double)
MAKE_MATRIX_CREATE(matrixd, double)
MAKE_MATRIX_GET(matrixd, double)
MAKE_MATRIX_GET_PTR(matrixd, double)
MAKE_MATRIX_GET_PTR_C(matrixd, double)
MAKE_MATRIX_SET(matrixd, double)
MAKE_MATRIX_DESTROY(matrixd)
MAKE_MATRIX_NROWS(matrixd)
MAKE_MATRIX_NCOLS(matrixd)
MAKE_MATRIX_RESIZE(matrixd, double)

#endif
