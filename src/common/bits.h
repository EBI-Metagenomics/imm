#ifndef COMMON_BITS_H
#define COMMON_BITS_H

#include "common/math.h"
#include <stdbool.h>
#include <stdint.h>

#define BITS_PER_BYTE 8

#define BITS_TO_LONGS(nr) DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))

static inline void bits_clr(unsigned long *x, unsigned bit)
{
    *x &= ~(1UL << bit);
}
static inline bool bits_get(unsigned long *x, unsigned bit)
{
    return !!((*x >> bit) & 1UL);
}
static inline void bits_set(unsigned long *x, unsigned bit)
{
    *x |= 1UL << bit;
}
static inline unsigned bits_width(uint32_t v)
{
    return v ? ((unsigned)__builtin_clz(v) ^ 31) + 1 : 0;
}

#endif
