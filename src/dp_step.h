#ifndef DP_STEP_H
#define DP_STEP_H

#include "state.h"
#include <inttypes.h>
#include <limits.h>

struct dp_step
{
    uint16_t state;
    uint8_t  seq_len;
};

#define DP_STEP_INVALID_SEQ_LEN UINT8_MAX

static inline struct dp_step dp_step_invalid(void)
{
    return (struct dp_step){STATE_INVALID, DP_STEP_INVALID_SEQ_LEN};
}

#endif
