#ifndef IMM_MATRIX_PRT_H
#define IMM_MATRIX_PRT_H

#include "src/imm/hide.h"

struct matrix_ptr
{
    void** data;
    int    nrows;
    int    ncols;
};

HIDE struct matrix_ptr* matrix_ptr_create(int nrows, int ncols);
static inline void*     matrix_ptr_get(const struct matrix_ptr* matrix, int r, int c)
{
    return matrix->data[r * matrix->ncols + c];
}
static inline void matrix_ptr_set(struct matrix_ptr* matrix, int r, int c, void* v)
{
    matrix->data[r * matrix->ncols + c] = v;
}
HIDE void matrix_ptr_set_all(struct matrix_ptr* matrix, void* v);
HIDE void matrix_ptr_destroy(struct matrix_ptr* matrix);

#endif
