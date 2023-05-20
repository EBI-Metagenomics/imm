#ifndef IMM_MATRIXU16_H
#define IMM_MATRIXU16_H

#include "compiler.h"
#include <stdint.h>

struct imm_matrixu16
{
  uint16_t *data;
  unsigned rows;
  unsigned cols;
};

int imm_matrixu16_init(struct imm_matrixu16 *, unsigned rows, unsigned cols);
void imm_matrixu16_empty(struct imm_matrixu16 *);
void imm_matrixu16_set(struct imm_matrixu16 *, unsigned r, unsigned c,
                       uint16_t v);
void imm_matrixu16_cleanup(struct imm_matrixu16 const *);
int imm_matrixu16_resize(struct imm_matrixu16 *, unsigned rows, unsigned cols);

IMM_TEMPLATE uint16_t imm_matrixu16_get(struct imm_matrixu16 const *x,
                                        unsigned const r, unsigned const c)
{
  return x->data[r * x->cols + c];
}

#endif
