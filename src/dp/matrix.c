#include "dp/matrix.h"
#include "dp/state_table.h"

#define MAX_LOOKUP 16

void matrix_init(struct matrix *matrix, struct state_table const *tbl)
{
    matrix->state_col = xmalloc(sizeof(*matrix->state_col) * tbl->nstates);

    unsigned next_col = 0;
    for (unsigned i = 0; i < tbl->nstates; ++i)
    {
        unsigned const min = state_table_min_seqlen(tbl, i);
        unsigned const max = state_table_max_seqlen(tbl, i);
        matrix->state_col[i] = (int16_t)(next_col - min);
        next_col += (unsigned)(max - min + 1);
    }

    matrixf_init(&matrix->score, MAX_LOOKUP, next_col);
}

void matrix_deinit(struct matrix const *matrix)
{
    matrixf_deinit(&matrix->score);
    free((void *)matrix->state_col);
}
