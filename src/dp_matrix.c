#include "dp_matrix.h"
#include "imm/imm.h"

#define MAX_LOOKUP 16

struct dp_matrix* dp_matrix_create(struct dp_state_table const* states)
{
    struct dp_matrix* matrix = malloc(sizeof(*matrix));

    matrix->states = states;
    matrix->state_col = malloc(sizeof(*matrix->state_col) * dp_state_table_nstates(states));

    uint_fast16_t next_col = 0;
    for (uint_fast16_t i = 0; i < dp_state_table_nstates(states); ++i) {
        uint_fast8_t const min = dp_state_table_min_seq(states, i);
        uint_fast8_t const max = dp_state_table_max_seq(states, i);
        matrix->state_col[i] = (int16_t)(next_col - min);
        next_col += (uint_fast16_t)(max - min + 1);
    }

    matrix->score = matrixf_create(MAX_LOOKUP, next_col);

    return matrix;
}

void dp_matrix_destroy(struct dp_matrix const* matrix)
{
    matrixf_destroy(matrix->score);
    free_c(matrix->state_col);
    free_c(matrix);
}
