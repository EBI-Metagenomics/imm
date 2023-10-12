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
void imm_matrixf_fill(struct imm_matrixf *, float);
float *imm_matrixf_get_ptr(struct imm_matrixf const *, unsigned r, unsigned c);
float const *imm_matrixf_get_ptr_c(struct imm_matrixf const *, unsigned r,
                                   unsigned c);
void imm_matrixf_cleanup(struct imm_matrixf *);
int imm_matrixf_resize(struct imm_matrixf *, unsigned rows, unsigned cols);

IMM_PURE float imm_matrixf_get(struct imm_matrixf const *x, unsigned r,
                               unsigned c)
{
  return x->data[r * x->cols + c];
}

IMM_INLINE void imm_matrixf_set(struct imm_matrixf *x, unsigned r, unsigned c,
                                float v)
{
  x->data[r * x->cols + c] = v;
}

#endif
