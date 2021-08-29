#include "dp/matrix.h"
#include "dp/state_table.h"
#include "error.h"
#include "imm/rc.h"

#define MAX_LOOKUP (2 * IMM_STATE_MAX_SEQLEN)

enum imm_rc matrix_init(struct matrix *matrix,
                        struct imm_dp_state_table const *tbl)
{
    matrix->state_col = NULL;
    if (matrixf_init(&matrix->score, MAX_LOOKUP, 1))
        return error(IMM_OUTOFMEM, "failed to matrix init");

    return matrix_reset(matrix, tbl);
}

enum imm_rc matrix_reset(struct matrix *m, struct imm_dp_state_table const *tbl)
{
    unsigned n = tbl->nstates;
    m->state_col = realloc(m->state_col, sizeof(*m->state_col) * n);
    if (!m->state_col && n > 0)
        return error(IMM_OUTOFMEM, "failed to realloc");

    unsigned next_col = 0;
    for (unsigned i = 0; i < n; ++i)
    {
        unsigned min = state_table_span(tbl, i).min;
        unsigned max = state_table_span(tbl, i).max;
        m->state_col[i] = (int16_t)(next_col - min);
        next_col += (unsigned)(max - min + 1);
    }
    if (matrixf_resize(&m->score, MAX_LOOKUP, next_col))
        return error(IMM_OUTOFMEM, "failed to resize");
    return IMM_SUCCESS;
}

void matrix_del(struct matrix const *matrix)
{
    matrixf_deinit(&matrix->score);
    free((void *)matrix->state_col);
}
