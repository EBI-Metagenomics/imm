#include "matrixf.h"
#include "rc.h"
#include "reallocf.h"
#include <stdlib.h>

int imm_matrixf_init(struct imm_matrixf *x, unsigned rows, unsigned cols)
{
  x->data = malloc(sizeof(*x->data) * (rows * cols));
  if (!x->data) return IMM_ENOMEM;
  x->rows = rows;
  x->cols = cols;
  return 0;
}

void imm_matrixf_empty(struct imm_matrixf *x)
{
  x->data = NULL;
  x->rows = 0;
  x->cols = 0;
}

float *imm_matrixf_get_ptr(struct imm_matrixf const *x, unsigned r, unsigned c)
{
  return x->data + (r * x->cols + c);
}

float const *imm_matrixf_get_ptr_c(struct imm_matrixf const *x, unsigned r,
                                   unsigned c)
{
  return x->data + (r * x->cols + c);
}

void imm_matrixf_deinit(struct imm_matrixf const *x) { free(x->data); }

int imm_matrixf_resize(struct imm_matrixf *x, unsigned rows, unsigned cols)
{
  x->data = imm_reallocf(x->data, sizeof(*x->data) * (rows * cols));
  if (!x->data && rows * cols > 0) return IMM_ENOMEM;
  x->rows = rows;
  x->cols = cols;
  return 0;
}
