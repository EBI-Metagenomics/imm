#include "dp_matrix.h"
#include "dp.h"
#include <string.h>

struct dp_matrix* dp_matrix_new(struct dp const* dp)
{
    struct dp_matrix* matrix = malloc(sizeof(struct dp_matrix));

    matrix->seq = NULL;
    matrix->state_col = malloc(sizeof(unsigned) * (dp->nstates));
    matrix->nstates = dp->nstates;

    unsigned next_col = 0;
    for (unsigned i = 0; i < dp->nstates; ++i) {
        matrix->state_col[i] = next_col;
        next_col += dp->states[i].max_seq - dp->states[i].min_seq + 1;
    }

    matrix->cell = matrix_cell_create(0, next_col);

    return matrix;
}

void dp_matrix_set(struct dp_matrix* matrix, struct imm_seq const* seq)
{
    matrix_cell_resize(matrix->cell, imm_seq_length(seq) + 1, matrix_cell_ncols(matrix->cell));
    matrix->seq = seq;
}

void dp_matrix_destroy(struct dp_matrix const* matrix)
{
    matrix_cell_destroy(matrix->cell);
    free_c(matrix->state_col);
    free_c(matrix);
}
