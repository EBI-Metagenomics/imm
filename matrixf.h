#ifndef IMM_MATRIXF_H
#define IMM_MATRIXF_H

#include "compiler.h"

struct imm_matrixf
{
  float *data;
  unsigned rows;
  unsigned cols;
};

int imm_matrixf_init(struct imm_matrixf *, unsigned rows, unsigned cols);
void imm_matrixf_empty(struct imm_matrixf *);
float *imm_matrixf_get_ptr(struct imm_matrixf const *, unsigned r, unsigned c);
float const *imm_matrixf_get_ptr_c(struct imm_matrixf const *, unsigned r,
                                   unsigned c);
void imm_matrixf_deinit(struct imm_matrixf const *);
int imm_matrixf_resize(struct imm_matrixf *, unsigned rows, unsigned cols);

IMM_TEMPLATE float imm_matrixf_get(struct imm_matrixf const *x,
                                   unsigned const r, unsigned const c)
{
  return x->data[r * x->cols + c];
}

IMM_TEMPLATE void imm_matrixf_set(struct imm_matrixf *x, unsigned const r,
                                  unsigned const c, float const v)
{
  x->data[r * x->cols + c] = v;
}

#endif
