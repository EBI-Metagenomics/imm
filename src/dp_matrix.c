#include "dp_matrix.h"
#include "dp.h"
#include <string.h>

struct dp_matrix* dp_matrix_create(struct dp const* dp, struct imm_seq const* seq)
{
    struct dp_matrix* matrix = malloc(sizeof(struct dp_matrix));

    matrix->seq = seq;
    matrix->state_col = malloc(sizeof(unsigned) * (dp->nstates));
    matrix->nstates = dp->nstates;

    unsigned next_col = 0;
    for (unsigned i = 0; i < dp->nstates; ++i) {
        matrix->state_col[i] = next_col;
        next_col += dp->states[i].max_seq - dp->states[i].min_seq + 1;
    }

    matrix->cell = matrix_cell_create(imm_seq_length(matrix->seq) + 1, next_col);

    return matrix;
}

struct dp_matrix* dp_matrix_clone(struct dp_matrix const* matrix)
{
    struct dp_matrix* new_matrix = malloc(sizeof(struct dp_matrix));

    new_matrix->seq = matrix->seq;
    new_matrix->state_col = malloc(sizeof(unsigned) * matrix->nstates);
    new_matrix->nstates = matrix->nstates;

    memcpy(new_matrix->state_col, matrix->state_col, sizeof(unsigned) * matrix->nstates);
    new_matrix->cell =
        matrix_cell_create(matrix_cell_nrows(matrix->cell), matrix_cell_ncols(matrix->cell));

    return new_matrix;
}

void dp_matrix_destroy(struct dp_matrix const* matrix)
{
    matrix_cell_destroy(matrix->cell);
    free_c(matrix->state_col);
    free_c(matrix);
}
