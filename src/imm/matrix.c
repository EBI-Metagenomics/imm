#include "src/imm/matrix.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

struct matrix *matrix_create(int nrows, int ncols)
{
    struct matrix *matrix = malloc(sizeof(struct matrix));
    matrix->data = malloc(sizeof(double) * ((size_t)(nrows * ncols)));
    matrix->nrows = nrows;
    matrix->ncols = ncols;
    return matrix;
}

void matrix_set_all(struct matrix *matrix, double v)
{
    for (int i = 0; i < matrix->nrows * matrix->ncols; ++i)
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

void matrix_print(const struct matrix *matrix)
{
    for (int r = 0; r < matrix->nrows; ++r) {
        printf("|");
        for (int c = 0; c < matrix->ncols - 1; ++c) {
            printf("%0.4f, ", exp(matrix_get(matrix, r, c)));
        }
        printf("%0.4f|\n", exp(matrix_get(matrix, r, matrix->ncols - 1)));
    }
}
