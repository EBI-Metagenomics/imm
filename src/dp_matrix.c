#include "dp_matrix.h"
#include "imath.h"
#include "imm/seq.h"

struct dp_matrix* dp_matrix_create(struct dp_state_table const* states)
{
    struct dp_matrix* matrix = malloc(sizeof(*matrix));

    matrix->states = states;
    matrix->state_col = malloc(sizeof(*matrix->state_col) * dp_state_table_nstates(states));

    uint32_t next_col = 0;
    for (uint32_t i = 0; i < dp_state_table_nstates(states); ++i) {
        matrix->state_col[i] = panic_sub_ui32(next_col, dp_state_table_min_seq(states, i));
        next_col += dp_state_table_max_seq(states, i) - dp_state_table_min_seq(states, i) + 1;
    }

    matrix->score = matrixd_create(1, next_col);
    matrix->prev_step = step_matrix_create(1, dp_state_table_nstates(states));

    return matrix;
}

void dp_matrix_destroy(struct dp_matrix const* matrix)
{
    matrixd_destroy(matrix->score);
    step_matrix_destroy(matrix->prev_step);
    free_c(matrix->state_col);
    free_c(matrix);
}

void dp_matrix_setup(struct dp_matrix* matrix, struct eseq const* eseq)
{
    unsigned seq_len = eseq_length(eseq);
    matrixd_resize(matrix->score, seq_len + 1, matrixd_ncols(matrix->score));
    step_matrix_resize(matrix->prev_step, seq_len + 1, step_matrix_ncols(matrix->prev_step));
}
