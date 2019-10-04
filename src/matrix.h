#ifndef IMM_MATRIX_H
#define IMM_MATRIX_H

#include <stddef.h>

struct matrix
{
    double *data;
    size_t nrows;
    size_t ncols;
};

struct matrix *matrix_create(size_t nrows, size_t ncols);
inline static double matrix_get(const struct matrix *matrix, size_t r, size_t c)
{
    return matrix->data[r * matrix->ncols + c];
}
inline static void matrix_set(struct matrix *matrix, size_t r, size_t c, double v)
{
    matrix->data[r * matrix->ncols + c] = v;
}
void matrix_set_all(struct matrix *matrix, double v);
void matrix_destroy(struct matrix *matrix);

#endif
