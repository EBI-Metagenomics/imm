#include "matrix.h"
#include <stdlib.h>

struct matrix *matrix_create(size_t nrows, size_t ncols)
{
    struct matrix *matrix = malloc(sizeof(struct matrix));
    matrix->data = malloc(sizeof(double) * nrows * ncols);
    matrix->nrows = nrows;
    matrix->ncols = ncols;
    return matrix;
}

void matrix_set_all(struct matrix *matrix, double v)
{
    for (size_t i = 0; i < matrix->nrows * matrix->ncols; ++i)
        matrix->data[i] = v;
}

void matrix_destroy(struct matrix *matrix)
{
    if (!matrix)
        return;

    free(matrix->data);
    matrix->data = NULL;
    matrix->nrows = 0;
    matrix->ncols = 0;
    free(matrix);
}
