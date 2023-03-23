#include "imm/matrixu16.h"
#include "error.h"
#include "imm/rc.h"
#include <stdlib.h>

int imm_matrixu16_init(struct imm_matrixu16 *x, unsigned rows, unsigned cols)
{
    x->data = malloc(sizeof(*x->data) * (rows * cols));
    if (!x->data) return error(IMM_NOMEM);
    x->rows = rows;
    x->cols = cols;
    return 0;
}

void imm_matrixu16_empty(struct imm_matrixu16 *x)
{
    x->data = ((void *)0);
    x->rows = 0;
    x->cols = 0;
}

uint16_t imm_matrixu16_get(struct imm_matrixu16 const *x, unsigned r,
                           unsigned c)
{
    return x->data[r * x->cols + c];
}

uint16_t *imm_matrixu16_get_ptr(struct imm_matrixu16 const *x, unsigned r,
                                unsigned c)
{
    return x->data + (r * x->cols + c);
}

uint16_t const *imm_matrixu16_get_ptr_c(struct imm_matrixu16 const *x,
                                        unsigned r, unsigned c)
{
    return x->data + (r * x->cols + c);
}

void imm_matrixu16_set(struct imm_matrixu16 *x, unsigned r, unsigned c,
                       uint16_t v)
{
    x->data[r * x->cols + c] = v;
}

void imm_matrixu16_deinit(struct imm_matrixu16 const *x) { free(x->data); }

int imm_matrixu16_resize(struct imm_matrixu16 *x, unsigned rows, unsigned cols)
{
    x->data = reallocf(x->data, sizeof(*x->data) * (rows * cols));
    if (!x->data && rows * cols > 0) return error(IMM_NOMEM);
    x->rows = rows;
    x->cols = cols;
    return 0;
}
