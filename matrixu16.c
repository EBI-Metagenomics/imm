#include "matrixu16.h"
#include "rc.h"
#include "reallocf.h"
#include <stdlib.h>

int imm_matrixu16_init(struct imm_matrixu16 *x, unsigned rows, unsigned cols)
{
  x->data = malloc(sizeof(*x->data) * (rows * cols));
  if (!x->data) return IMM_ENOMEM;
  x->rows = rows;
  x->cols = cols;
  return 0;
}

void imm_matrixu16_empty(struct imm_matrixu16 *x)
{
  x->data = NULL;
  x->rows = 0;
  x->cols = 0;
}

void imm_matrixu16_set(struct imm_matrixu16 *x, unsigned r, unsigned c,
                       uint16_t v)
{
  x->data[r * x->cols + c] = v;
}

void imm_matrixu16_cleanup(struct imm_matrixu16 const *x) { free(x->data); }

int imm_matrixu16_resize(struct imm_matrixu16 *x, unsigned rows, unsigned cols)
{
  x->data = imm_reallocf(x->data, sizeof(*x->data) * (rows * cols));
  if (!x->data && rows * cols > 0) return IMM_ENOMEM;
  x->rows = rows;
  x->cols = cols;
  return 0;
}
