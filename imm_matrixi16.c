#include "imm_matrixi16.h"
#include "imm_rc.h"
#include "imm_reallocf.h"
#include <stdlib.h>

void imm_matrixi16_init(struct imm_matrixi16 *x)
{
  x->data = NULL;
  x->rows = 0;
  x->cols = 0;
}

void imm_matrixi16_empty(struct imm_matrixi16 *x)
{
  imm_matrixi16_cleanup(x);
  x->rows = 0;
  x->cols = 0;
}

void imm_matrixi16_set(struct imm_matrixi16 *x, int r, int c, int16_t v)
{
  x->data[r * x->cols + c] = v;
}

void imm_matrixi16_cleanup(struct imm_matrixi16 *x)
{
  free(x->data);
  x->data = NULL;
}

int imm_matrixi16_resize(struct imm_matrixi16 *x, int rows, int cols)
{
  if (rows <= 0 || cols <= 0) return IMM_EINVAL;
  size_t size = sizeof(*x->data) * ((size_t)rows * (size_t)cols);
  x->data = imm_reallocf(x->data, size);
  if (!x->data) return IMM_ENOMEM;
  x->rows = rows;
  x->cols = cols;
  return 0;
}
