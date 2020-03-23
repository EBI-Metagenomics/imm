#ifndef IMM_DP_STEP_H
#define IMM_DP_STEP_H

#include "matrix.h"

struct dp_step
{
    unsigned state;
    unsigned seq_len;
};

/* Defines `struct dp_step_matrix` type. */
MAKE_MATRIX_STRUCT(dp_step_matrix, struct dp_step)
MAKE_MATRIX_CREATE(dp_step_matrix, struct dp_step)
MAKE_MATRIX_GET(dp_step_matrix, struct dp_step)
MAKE_MATRIX_GET_C(dp_step_matrix, struct dp_step)
MAKE_MATRIX_DESTROY(dp_step_matrix)
MAKE_MATRIX_NROWS(dp_step_matrix)
MAKE_MATRIX_NCOLS(dp_step_matrix)
MAKE_MATRIX_RESIZE(dp_step_matrix, struct dp_step)

#endif
