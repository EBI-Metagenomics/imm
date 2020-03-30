#include "dp_matrix.h"
#include "imath.h"
#include "imm/seq.h"

struct dp_matrix* dp_matrix_new(struct dp_states const* states)
{
    struct dp_matrix* matrix = malloc(sizeof(struct dp_matrix));

    matrix->states = states;
    matrix->state_col = malloc(sizeof(int) * dp_states_nstates(states));

    unsigned next_col = 0;
    for (unsigned i = 0; i < dp_states_nstates(states); ++i) {
        matrix->state_col[i] = panic_sub_ui(next_col, dp_states_min_seq(states, i));
        next_col += dp_states_max_seq(states, i) - dp_states_min_seq(states, i) + 1;
    }

    matrix->score = matrixd_create(1, next_col);
    matrix->prev_step = step_matrix_create(1, dp_states_nstates(states));

    return matrix;
}

void dp_matrix_setup(struct dp_matrix* matrix, struct eseq const* eseq)
{
    unsigned seq_len = eseq_length(eseq);
    matrixd_resize(matrix->score, seq_len + 1, matrixd_ncols(matrix->score));
    step_matrix_resize(matrix->prev_step, seq_len + 1, step_matrix_ncols(matrix->prev_step));
}

void dp_matrix_destroy(struct dp_matrix const* matrix)
{
    matrixd_destroy(matrix->score);
    step_matrix_destroy(matrix->prev_step);
    free_c(matrix->state_col);
    free_c(matrix);
}
