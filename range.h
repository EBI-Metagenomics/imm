#ifndef IMM_RANGE_H
#define IMM_RANGE_H

#include "export.h"
#include <stdbool.h>
#include <stdio.h>

// Right-open interval: [start, stop)
struct imm_range
{
  unsigned start;
  unsigned stop;
};

IMM_API struct imm_range imm_range(unsigned start, unsigned stop);
IMM_API void imm_range_set(struct imm_range *, unsigned start, unsigned stop);
IMM_API unsigned imm_range_size(struct imm_range);
IMM_API bool imm_range_empty(struct imm_range);
IMM_API void imm_range_swap(struct imm_range *, struct imm_range *);
IMM_API struct imm_range imm_range_intersect(struct imm_range,
                                             struct imm_range);
IMM_API void imm_range_subtract(struct imm_range, struct imm_range,
                                struct imm_range *, struct imm_range *);
IMM_API void imm_range_dump(struct imm_range, FILE *restrict);

#endif
