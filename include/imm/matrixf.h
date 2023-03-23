#ifndef IMM_MATRIXF_H
#define IMM_MATRIXF_H

#include "imm/float.h"

struct imm_matrixf
{
    imm_float *data;
    unsigned rows;
    unsigned cols;
};

int imm_matrixf_init(struct imm_matrixf *, unsigned rows, unsigned cols);
void imm_matrixf_empty(struct imm_matrixf *);
imm_float imm_matrixf_get(struct imm_matrixf const *, unsigned r, unsigned c);
imm_float *imm_matrixf_get_ptr(struct imm_matrixf const *, unsigned r,
                               unsigned c);
imm_float const *imm_matrixf_get_ptr_c(struct imm_matrixf const *, unsigned r,
                                       unsigned c);
void imm_matrixf_set(struct imm_matrixf *, unsigned r, unsigned c, imm_float v);
void imm_matrixf_deinit(struct imm_matrixf const *);
int imm_matrixf_resize(struct imm_matrixf *, unsigned rows, unsigned cols);

#endif
