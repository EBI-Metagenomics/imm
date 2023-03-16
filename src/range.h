#ifndef RANGE_H
#define RANGE_H

#include <stdbool.h>
#include <stdio.h>

// [a, b)
struct imm_range
{
    unsigned a;
    unsigned b;
};

struct imm_range imm_range_init(unsigned a, unsigned b);
void imm_range_set(struct imm_range *x, unsigned a, unsigned b);
unsigned imm_range_size(struct imm_range x);
bool imm_range_empty(struct imm_range x);
void imm_range_swap(struct imm_range *, struct imm_range *);
struct imm_range imm_range_intersect(struct imm_range x, struct imm_range y);
void imm_range_subtract(struct imm_range x, struct imm_range y,
                        struct imm_range *l, struct imm_range *r);
void imm_range_dump(struct imm_range, FILE *restrict);

#endif
