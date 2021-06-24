#include "dp/matrix.h"
#include "dp/state_table.h"
#include "support.h"

#define MAX_LOOKUP (2 * IMM_STATE_MAX_SEQLEN)

void matrix_init(struct matrix *matrix, struct state_table const *tbl)
{
    matrix->state_col = NULL;
    matrixf_init(&matrix->score, MAX_LOOKUP, 1);
    matrix_reset(matrix, tbl);
}

void matrix_reset(struct matrix *matrix, struct state_table const *tbl)
{
    matrix->state_col =
        xrealloc(matrix->state_col, sizeof(*matrix->state_col) * tbl->nstates);

    unsigned next_col = 0;
    for (unsigned i = 0; i < tbl->nstates; ++i)
    {
        unsigned min = state_table_span(tbl, i).min;
        unsigned max = state_table_span(tbl, i).max;
        matrix->state_col[i] = (int16_t)(next_col - min);
        next_col += (unsigned)(max - min + 1);
    }
    matrixf_resize(&matrix->score, MAX_LOOKUP, next_col);
}

void matrix_del(struct matrix const *matrix)
{
    matrixf_deinit(&matrix->score);
    free((void *)matrix->state_col);
}
