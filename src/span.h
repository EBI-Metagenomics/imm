#ifndef DP_SPAN_H
#define DP_SPAN_H

#include "imm/state_types.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

struct span
{
    imm_state_seqlen_t min;
    imm_state_seqlen_t max;
} __attribute__((__packed__));

#define SPAN(min, max)                                                         \
    (struct span)                                                              \
    {                                                                          \
        (imm_state_seqlen_t)(min), (imm_state_seqlen_t)(max)                   \
    }

static_assert(sizeof(struct span) == 2, "struct span must be packed");

static inline struct span span_init(unsigned min, unsigned max)
{
    return (struct span){.min = min, .max = max};
}

static inline uint16_t span_zip(struct span const *span)
{
    return (uint16_t)((span->min << 8) | span->max);
}

static inline bool span_unzip(struct span span[static 1], uint16_t v)
{
    span->max = (imm_state_seqlen_t)v;
    span->min = (imm_state_seqlen_t)(v >> 8);
    return span->min <= span->max;
}

#endif
