#include "matrix.h"
#include "fmt.h"
#include "matrixf.h"
#include "rc.h"
#include "reallocf.h"
#include "state.h"
#include "state_table.h"
#include "zspan.h"
#include <stdlib.h>

int imm_matrix_init(struct imm_matrix *x, struct imm_state_table const *tbl)
{
  int rc = 0;
  x->state_col = NULL;
  if ((rc = imm_matrixf_init(&x->score, IMM_MATRIX_NROWS, 1))) return rc;
  return imm_matrix_reset(x, tbl);
}

int imm_matrix_reset(struct imm_matrix *x, struct imm_state_table const *tbl)
{
  unsigned n = tbl->nstates;
  x->state_col = imm_reallocf(x->state_col, sizeof(*x->state_col) * n);
  if (!x->state_col && n > 0) return IMM_ENOMEM;

  unsigned next_col = 0;
  for (unsigned i = 0; i < n; ++i)
  {
    unsigned min = imm_zspan_min(imm_state_table_span(tbl, i));
    unsigned max = imm_zspan_max(imm_state_table_span(tbl, i));
    x->state_col[i] = (int16_t)(next_col - min);
    next_col += (unsigned)(max - min + 1);
  }
  int rc = 0;
  if ((rc = imm_matrixf_resize(&x->score, IMM_MATRIX_NROWS, next_col)))
    return rc;
  return rc;
}

void imm_matrix_cleanup(struct imm_matrix *x)
{
  if (x)
  {
    imm_matrixf_deinit(&x->score);
    free((void *)x->state_col);
    x->state_col = NULL;
  }
}

void imm_matrix_dump(struct imm_matrix *matrix, FILE *restrict fp)
{
  for (unsigned r = 0; r < IMM_MATRIX_NROWS; ++r)
  {
    for (unsigned c = 0; c < matrix->score.cols; ++c)
    {
      if (c > 0) fputc(',', fp);
      fprintf(fp, IMM_FMT_F32, imm_matrixf_get(&matrix->score, r, c));
    }
    fputc('\n', fp);
  }
}
