#include "imm/matrix.h"
#include "error.h"
#include "imm/matrixf.h"
#include "imm/rc.h"
#include "imm/state_table.h"
#include "reallocf.h"
#include "span.h"
#include <stdlib.h>

#define MAX_LOOKUP (2 * IMM_STATE_MAX_SEQLEN)

int imm_matrix_init(struct imm_matrix *x, struct imm_state_table const *tbl)
{
    int rc = 0;
    x->state_col = NULL;
    if ((rc = imm_matrixf_init(&x->score, MAX_LOOKUP, 1))) return rc;
    return imm_matrix_reset(x, tbl);
}

int imm_matrix_reset(struct imm_matrix *x, struct imm_state_table const *tbl)
{
    unsigned n = tbl->nstates;
    x->state_col = imm_reallocf(x->state_col, sizeof(*x->state_col) * n);
    if (!x->state_col && n > 0) return IMM_NOMEM;

    unsigned next_col = 0;
    for (unsigned i = 0; i < n; ++i)
    {
        unsigned min = imm_state_table_span(tbl, i).min;
        unsigned max = imm_state_table_span(tbl, i).max;
        x->state_col[i] = (int)(next_col - min);
        next_col += (unsigned)(max - min + 1);
    }
    int rc = 0;
    if ((rc = imm_matrixf_resize(&x->score, MAX_LOOKUP, next_col))) return rc;
    return rc;
}

void imm_matrix_del(struct imm_matrix const *x)
{
    if (x)
    {
        imm_matrixf_deinit(&x->score);
        free((void *)x->state_col);
    }
}

void imm_matrix_dump(struct imm_matrix *matrix, FILE *restrict fp)
{
    for (unsigned r = 0; r < matrix->score.rows; ++r)
    {
        for (unsigned c = 0; c < matrix->score.cols; ++c)
        {
            if (c > 0) fputc(',', fp);
            fprintf(fp, "%f", imm_matrixf_get(&matrix->score, r, c));
        }
        fputc('\n', fp);
    }
}

imm_float imm_matrix_get_score(struct imm_matrix const *x, struct imm_cell y)
{
    y.row = y.row % x->score.rows;
    int col = x->state_col[y.state] + (int)y.len;
    return imm_matrixf_get(&x->score, y.row, (unsigned)col);
}

void imm_matrix_set_score(struct imm_matrix *x, struct imm_cell y,
                          imm_float score)
{
    y.row = y.row % x->score.rows;
    int col = x->state_col[y.state] + (int)y.len;
    imm_matrixf_set(&x->score, y.row, (unsigned)col, score);
}
