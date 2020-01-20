#include "matrix.h"
#include "free.h"
#include "imm/imm.h"
#include <stdlib.h>

struct matrix* imm_matrix_create(int nrows, int ncols)
{
    struct matrix* matrix = malloc(sizeof(struct matrix));
    matrix->data = malloc(sizeof(double) * ((size_t)(nrows * ncols)));
    matrix->nrows = nrows;
    matrix->ncols = ncols;
    return matrix;
}

void imm_matrix_fill(struct matrix* matrix, double v)
{
    for (int i = 0; i < matrix->nrows * matrix->ncols; ++i)
        matrix->data[i] = v;
}

void imm_matrix_destroy(struct matrix* matrix)
{
    if (!matrix) {
        imm_error("matrix should not be null");
        return;
    }

    free_c(matrix->data);
    matrix->data = NULL;
    matrix->nrows = 0;
    matrix->ncols = 0;
    free_c(matrix);
}
