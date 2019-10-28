#ifndef IMM_MATRIX_H
#define IMM_MATRIX_H

#include "src/imm/hide.h"

struct matrix
{
    double* data;
    int     nrows;
    int     ncols;
};

HIDE struct matrix*  matrix_create(int nrows, int ncols);
inline static double matrix_get(struct matrix const* matrix, int r, int c)
{
    return matrix->data[r * matrix->ncols + c];
}
inline static void matrix_set(struct matrix* matrix, int r, int c, double v)
{
    matrix->data[r * matrix->ncols + c] = v;
}
HIDE void matrix_set_all(struct matrix* matrix, double v);
HIDE void matrix_destroy(struct matrix* matrix);

#endif
