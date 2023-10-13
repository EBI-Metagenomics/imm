#include "matrixu16.h"
#include "rc.h"
#include "reallocf.h"
#include <stdlib.h>

int imm_matrixu16_init(struct imm_matrixu16 *x, int rows, int cols)
{
  x->data = malloc(sizeof(*x->data) * (size_t)(rows * cols));
  if (!x->data) return IMM_ENOMEM;
  x->rows = rows;
  x->cols = cols;
  return 0;
}

void imm_matrixu16_empty(struct imm_matrixu16 *x)
{
  imm_matrixu16_cleanup(x);
  x->rows = 0;
  x->cols = 0;
}

void imm_matrixu16_set(struct imm_matrixu16 *x, int r, int c, int16_t v)
{
  x->data[r * x->cols + c] = v;
}

void imm_matrixu16_cleanup(struct imm_matrixu16 *x)
{
  free(x->data);
  x->data = NULL;
}

int imm_matrixu16_resize(struct imm_matrixu16 *x, int rows, int cols)
{
  x->data = imm_reallocf(x->data, sizeof(*x->data) * (size_t)(rows * cols));
  if (!x->data && rows * cols > 0) return IMM_ENOMEM;
  x->rows = rows;
  x->cols = cols;
  return 0;
}
