#ifndef IMM_MATRIX_H
#define IMM_MATRIX_H

struct matrix
{
    double* data;
    int     nrows;
    int     ncols;
};

struct matrix*       imm_matrix_create(int nrows, int ncols);
static inline double imm_matrix_get(struct matrix const* matrix, int r, int c)
{
    return matrix->data[r * matrix->ncols + c];
}
static inline void imm_matrix_set(struct matrix* matrix, int r, int c, double v)
{
    matrix->data[r * matrix->ncols + c] = v;
}
void imm_matrix_fill(struct matrix* matrix, double v);
void imm_matrix_destroy(struct matrix* matrix);

#endif
