#include "src/imm/matrix_ptr.h"
#include <stdlib.h>

struct matrix_ptr *matrix_ptr_create(int nrows, int ncols)
{
    struct matrix_ptr *matrix = malloc(sizeof(struct matrix_ptr));
    matrix->data = malloc(sizeof(void *) * ((size_t)(nrows * ncols)));
    matrix->nrows = nrows;
    matrix->ncols = ncols;
    return matrix;
}

void matrix_ptr_set_all(struct matrix_ptr *matrix, void *v)
{
    for (int i = 0; i < matrix->nrows * matrix->ncols; ++i)
        matrix->data[i] = v;
}

void matrix_ptr_destroy(struct matrix_ptr *matrix)
{
    if (!matrix)
        return;

    free(matrix->data);
    matrix->data = NULL;
    matrix->nrows = 0;
    matrix->ncols = 0;
    free(matrix);
}
