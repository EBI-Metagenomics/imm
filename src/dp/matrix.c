#include "dp/matrix.h"
#include "dp/state_table.h"
#include "error.h"
#include "imm/rc.h"
#include "reallocf.h"

#define MAX_LOOKUP (2 * IMM_STATE_MAX_SEQLEN)

enum imm_rc imm_matrix_init(struct matrix *matrix,
                            struct imm_dp_state_table const *tbl)
{
    matrix->state_col = NULL;
    if (matrixf_init(&matrix->score, MAX_LOOKUP, 1)) return error(IMM_NOMEM);

    return imm_matrix_reset(matrix, tbl);
}

enum imm_rc imm_matrix_reset(struct matrix *m,
                             struct imm_dp_state_table const *tbl)
{
    unsigned n = tbl->nstates;
    m->state_col = reallocf(m->state_col, sizeof(*m->state_col) * n);
    if (!m->state_col && n > 0) return error(IMM_NOMEM);

    unsigned next_col = 0;
    for (unsigned i = 0; i < n; ++i)
    {
        unsigned min = state_table_span(tbl, i).min;
        unsigned max = state_table_span(tbl, i).max;
        m->state_col[i] = (int)(next_col - min);
        next_col += (unsigned)(max - min + 1);
    }
    if (matrixf_resize(&m->score, MAX_LOOKUP, next_col))
        return error(IMM_NOMEM);
    return IMM_OK;
}

void imm_matrix_del(struct matrix const *matrix)
{
    matrixf_deinit(&matrix->score);
    free((void *)matrix->state_col);
}
