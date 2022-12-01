#ifndef IMM_SPAN_H
#define IMM_SPAN_H

#include <assert.h>

struct imm_span
{
    unsigned min;
    unsigned max;
};

static_assert(sizeof(struct imm_span) == 8, "struct imm_span must be packed");

#define imm_span(min, max)                                                     \
    (struct imm_span)                                                          \
    {                                                                          \
        (min), (max)                                                           \
    }

#endif
