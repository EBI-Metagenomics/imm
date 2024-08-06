#include "imm_matrix.h"
#include "imm_fmt.h"
#include "imm_lprob.h"
#include "imm_matrixf.h"
#include "imm_rc.h"
#include "imm_reallocf.h"
#include "imm_state.h"
#include "imm_state_table.h"
#include "imm_zspan.h"
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
  x->state_table = tbl;
  int n = tbl->nstates;
  x->state_col = imm_reallocf(x->state_col, sizeof(*x->state_col) * (size_t)n);
  if (!x->state_col && n > 0) return IMM_ENOMEM;

  int next_col = 0;
  for (int i = 0; i < n; ++i)
  {
    int min = imm_zspan_min(imm_state_table_zspan(tbl, i));
    int max = imm_zspan_max(imm_state_table_zspan(tbl, i));
    x->state_col[i] = (int16_t)(next_col - min);
    next_col += max - min + 1;
  }
  int rc = 0;
  if ((rc = imm_matrixf_resize(&x->score, IMM_MATRIX_NROWS, next_col)))
    return rc;
  return rc;
}

void imm_matrix_prepare(struct imm_matrix *x)
{
  imm_matrixf_fill(&x->score, IMM_LPROB_NAN);
}

void imm_matrix_cleanup(struct imm_matrix *x)
{
  if (x)
  {
    imm_matrixf_cleanup(&x->score);
    x->state_table = NULL;
    free((void *)x->state_col);
    x->state_col = NULL;
  }
}

void imm_matrix_dump(struct imm_matrix const *x, FILE *restrict fp)
{
  // Header
  int c = 0;
  for (int i = 0; i < x->state_table->nstates; ++i)
  {
    struct imm_range range = imm_state_table_range(x->state_table, i);
    for (int j = range.start; j < range.stop; ++j)
    {
      if (c > 0) fputc(',', fp);
      fprintf(fp, "%s:%d", imm_state_table_name(x->state_table, i), j);
      c++;
    }
  }
  fputc('\n', fp);

  // Body
  for (int r = 0; r < IMM_MATRIX_NROWS; ++r)
  {
    int c = 0;
    for (int i = 0; i < x->state_table->nstates; ++i)
    {
      struct imm_range range = imm_state_table_range(x->state_table, i);
      for (int j = range.start; j < range.stop; ++j)
      {
        if (c > 0) fputc(',', fp);
        struct imm_cell cell = imm_cell(r, (int_fast16_t)i, (int_fast8_t)j);
        fprintf(fp, imm_f32(), imm_matrix_get_score(x, cell));
        c++;
      }
    }
    fputc('\n', fp);
  }
}
