#ifndef DP_STEP_H
#define DP_STEP_H

#include <inttypes.h>
#include <limits.h>

struct dp_step
{
    uint16_t state;
    uint8_t  seq_len;
};

static inline struct dp_step dp_step_invalid(void)
{
    return (struct dp_step){UINT16_MAX, UINT8_MAX};
}

#endif
