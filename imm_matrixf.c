#include "imm_matrixf.h"
#include "imm_rc.h"
#include "imm_reallocf.h"
#include <stdlib.h>

int imm_matrixf_init(struct imm_matrixf *x, int rows, int cols)
{
  x->data = malloc(sizeof(*x->data) * (size_t)(rows * cols));
  if (!x->data) return IMM_ENOMEM;
  x->rows = rows;
  x->cols = cols;
  return 0;
}

void imm_matrixf_empty(struct imm_matrixf *x)
{
  imm_matrixf_cleanup(x);
  x->rows = 0;
  x->cols = 0;
}

void imm_matrixf_fill(struct imm_matrixf *x, float v)
{
  for (int i = 0; i < x->rows * x->cols; ++i)
    x->data[i] = v;
}

float *imm_matrixf_get_ptr(struct imm_matrixf const *x, int r, int c)
{
  return x->data + (r * x->cols + c);
}

float const *imm_matrixf_get_ptr_c(struct imm_matrixf const *x, int r, int c)
{
  return x->data + (r * x->cols + c);
}

void imm_matrixf_cleanup(struct imm_matrixf *x)
{
  free(x->data);
  x->data = NULL;
}

int imm_matrixf_resize(struct imm_matrixf *x, int rows, int cols)
{
  x->data = imm_reallocf(x->data, sizeof(*x->data) * (size_t)(rows * cols));
  if (!x->data && rows * cols > 0) return IMM_ENOMEM;
  x->rows = rows;
  x->cols = cols;
  return 0;
}
