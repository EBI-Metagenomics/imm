#ifndef DP_SPAN_H
#define DP_SPAN_H

#include "imm/state_types.h"
#include <assert.h>

struct span
{
    __imm_state_seqlen_t min;
    __imm_state_seqlen_t max;
} __attribute__((__packed__));

#define SPAN(min, max)                                                         \
    (struct span) { (__imm_state_seqlen_t)(min), (__imm_state_seqlen_t)(max) }

static_assert(sizeof(struct span) == 2, "struct span must be packed");

#endif
