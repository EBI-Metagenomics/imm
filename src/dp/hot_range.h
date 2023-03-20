#ifndef DP_HOT_RANGE_H
#define DP_HOT_RANGE_H

#include "span.h"

struct imm_dp;

struct hot_range
{
    unsigned left;
    unsigned start;
    unsigned total;
};

void imm_hot_range(struct imm_dp const *, struct span, struct hot_range *);

#endif
