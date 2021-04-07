#ifndef BITARR_H
#define BITARR_H

#include "util/bits.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define BITARR_LONG_START(bit) (bit / (sizeof(long) * BITS_PER_BYTE))
#define BITARR_BIT_START(bit) (bit % (sizeof(long) * BITS_PER_BYTE))

static inline unsigned long* bitarr_alloc(size_t bits) { return malloc(sizeof(long) * BITS_TO_LONGS(bits)); }
static inline unsigned long  bitarr_get(unsigned long* x, size_t start, unsigned len);
static inline unsigned long* bitarr_realloc(unsigned long* x, size_t bits);
static inline void           bitarr_set(unsigned long* x, unsigned long val, size_t start, unsigned len);

static inline unsigned long bitarr_get(unsigned long* x, size_t start, unsigned len)
{
    unsigned long val = 0;
    for (unsigned i = 0; i < len; ++i) {
        size_t j = start + (size_t)i;
        if (bits_get(x + BITARR_LONG_START(j), BITARR_BIT_START(j)))
            bits_set(&val, i);
        else
            bits_clr(&val, i);
    }
    return val;
}

static inline unsigned long* bitarr_realloc(unsigned long* x, size_t bits)
{
    return realloc(x, sizeof(long) * BITS_TO_LONGS(bits));
}

static inline void bitarr_set(unsigned long* x, unsigned long val, size_t start, unsigned len)
{
    for (unsigned i = 0; i < len; ++i) {
        size_t j = start + (size_t)i;
        if (bits_get(&val, i))
            bits_set(x + BITARR_LONG_START(j), BITARR_BIT_START(j));
        else
            bits_clr(x + BITARR_LONG_START(j), BITARR_BIT_START(j));
    }
}

#endif
