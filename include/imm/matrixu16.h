#ifndef IMM_MATRIXU16_H
#define IMM_MATRIXU16_H

#include <stdint.h>

struct imm_matrixu16
{
    uint16_t *data;
    unsigned rows;
    unsigned cols;
};

int imm_matrixu16_init(struct imm_matrixu16 *, unsigned rows, unsigned cols);
void imm_matrixu16_empty(struct imm_matrixu16 *);
uint16_t imm_matrixu16_get(struct imm_matrixu16 const *, unsigned r,
                           unsigned c);
uint16_t *imm_matrixu16_get_ptr(struct imm_matrixu16 const *, unsigned r,
                                unsigned c);
uint16_t const *imm_matrixu16_get_ptr_c(struct imm_matrixu16 const *,
                                        unsigned r, unsigned c);
void imm_matrixu16_set(struct imm_matrixu16 *, unsigned r, unsigned c,
                       uint16_t v);
void imm_matrixu16_deinit(struct imm_matrixu16 const *);
int imm_matrixu16_resize(struct imm_matrixu16 *, unsigned rows, unsigned cols);

#endif
