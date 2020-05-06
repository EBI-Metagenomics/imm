#ifndef STEP_MATRIX_H
#define STEP_MATRIX_H

#include "dp_step.h"
#include "make_matrix.h"

MAKE_MATRIX_STRUCT(step_matrix, struct dp_step)
MAKE_MATRIX_CREATE(step_matrix, struct dp_step)
MAKE_MATRIX_GET_PTR(step_matrix, struct dp_step)
MAKE_MATRIX_GET_PTR_C(step_matrix, struct dp_step)
MAKE_MATRIX_DESTROY(step_matrix)
MAKE_MATRIX_NROWS(step_matrix)
MAKE_MATRIX_NCOLS(step_matrix)
MAKE_MATRIX_RESIZE(step_matrix, struct dp_step)

#endif
