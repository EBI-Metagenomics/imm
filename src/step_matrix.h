#ifndef IMM_STEP_MATRIX_H
#define IMM_STEP_MATRIX_H

#include "make_matrix.h"
#include "dp2_step.h"

MAKE_MATRIX_STRUCT(step_matrix, struct dp2_step)
MAKE_MATRIX_CREATE(step_matrix, struct dp2_step)
MAKE_MATRIX_GET_PTR(step_matrix, struct dp2_step)
MAKE_MATRIX_GET_PTR_C(step_matrix, struct dp2_step)
MAKE_MATRIX_DESTROY(step_matrix)
MAKE_MATRIX_NROWS(step_matrix)
MAKE_MATRIX_NCOLS(step_matrix)
MAKE_MATRIX_RESIZE(step_matrix, struct dp2_step)

#endif
