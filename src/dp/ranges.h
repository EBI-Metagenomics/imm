#ifndef DP_RANGES_H
#define DP_RANGES_H

#include "imm/range.h"

struct ranges
{
    struct imm_range safe_future;
    struct imm_range safe;
    struct imm_range unsafe;
    struct imm_range safe_past;
};

void dp_ranges_find(struct ranges *x, unsigned len);

#endif
