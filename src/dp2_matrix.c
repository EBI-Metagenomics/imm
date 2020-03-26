#include "dp2_matrix.h"
#include "imm/seq.h"

struct dp2_matrix* dp2_matrix_new(struct dp2_states const* states)
{
    struct dp2_matrix* matrix = malloc(sizeof(struct dp2_matrix));

    matrix->states = states;
    matrix->seq = NULL;
    matrix->state_col = malloc(sizeof(unsigned) * dp2_states_nstates(states));

    unsigned next_col = 0;
    for (unsigned i = 0; i < dp2_states_nstates(states); ++i) {
        matrix->state_col[i] = next_col;
        next_col += dp2_states_max_seq(states, i) - dp2_states_min_seq(states, i) + 1;
    }

    matrix->cost = matrixd_create(1, next_col);
    matrix->prev_step = step_matrix_create(1, next_col);

    return matrix;
}

void dp2_matrix_setup(struct dp2_matrix* matrix, struct imm_seq const* seq)
{
    matrixd_resize(matrix->cost, imm_seq_length(seq) + 1, matrixd_ncols(matrix->cost));
    step_matrix_resize(matrix->prev_step, imm_seq_length(seq) + 1,
                       matrixd_ncols(matrix->cost));
    matrix->seq = seq;
}

void dp2_matrix_destroy(struct dp2_matrix const* matrix)
{
    matrixd_destroy(matrix->cost);
    step_matrix_destroy(matrix->prev_step);
    imm_free(matrix->state_col);
    imm_free(matrix);
}
