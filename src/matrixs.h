#ifndef MATRIXS_H
#define MATRIXS_H

#include "make_matrix.h"
#include "score.h"

/* Defines `struct matrixs` type. */
MAKE_MATRIX_STRUCT(matrixs, score_t)
MAKE_MATRIX_CREATE(matrixs, score_t)
MAKE_MATRIX_GET(matrixs, score_t)
MAKE_MATRIX_GET_PTR(matrixs, score_t)
MAKE_MATRIX_GET_PTR_C(matrixs, score_t)
MAKE_MATRIX_SET(matrixs, score_t)
MAKE_MATRIX_DESTROY(matrixs)
MAKE_MATRIX_NROWS(matrixs)
MAKE_MATRIX_NCOLS(matrixs)
MAKE_MATRIX_RESIZE(matrixs, score_t)

#endif
