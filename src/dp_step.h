#ifndef DP_STEP_H
#define DP_STEP_H

#include <limits.h>
#include <stdint.h>

struct dp_step
{
    uint32_t state;
    unsigned seq_len;
};

static inline struct dp_step dp_step_invalid(void) { return (struct dp_step){UINT32_MAX, UINT_MAX}; }

#endif
