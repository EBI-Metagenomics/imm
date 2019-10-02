#include "matrix.h"
#include <stdlib.h>

struct matrix *matrix_create(size_t nrows, size_t ncols)
{
    struct matrix *matrix = malloc(sizeof(double) * nrows * ncols);

    return matrix;
}

void matrix_destroy(struct matrix *matrix)
{
    if (!matrix)
        return;

    if (matrix->data)
        free(matrix->data);

    matrix->data = NULL;
    matrix->ncols = 0;

    free(matrix);
}
