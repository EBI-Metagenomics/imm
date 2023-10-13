#ifndef IMM_MATRIXU16_H
#define IMM_MATRIXU16_H

#include "compiler.h"
#include <stdint.h>

struct imm_matrixu16
{
  int16_t *data;
  int rows;
  int cols;
};

int imm_matrixu16_init(struct imm_matrixu16 *, int rows, int cols);
void imm_matrixu16_empty(struct imm_matrixu16 *);
void imm_matrixu16_set(struct imm_matrixu16 *, int r, int c, int16_t v);
void imm_matrixu16_cleanup(struct imm_matrixu16 *);
int imm_matrixu16_resize(struct imm_matrixu16 *, int rows, int cols);

IMM_PURE int16_t imm_matrixu16_get(struct imm_matrixu16 const *x, int r, int c)
{
  return x->data[r * x->cols + c];
}

#endif
