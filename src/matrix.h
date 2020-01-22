#ifndef IMM_MATRIX_H
#define IMM_MATRIX_H

struct matrix
{
    double* data;
    int     nrows;
    int     ncols;
};

struct matrix*       matrix_create(int nrows, int ncols);
static inline double matrix_get(struct matrix const* matrix, int r, int c)
{
    return matrix->data[r * matrix->ncols + c];
}
static inline void matrix_set(struct matrix* matrix, int r, int c, double v)
{
    matrix->data[r * matrix->ncols + c] = v;
}
void matrix_fill(struct matrix* matrix, double v);
void matrix_destroy(struct matrix* matrix);

#endif
