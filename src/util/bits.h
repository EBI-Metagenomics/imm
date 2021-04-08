#ifndef UTIL_BITS_H
#define UTIL_BITS_H

#include "util/const.h"
#include "util/math.h"
#include <stdbool.h>
#include <stdint.h>

#define BITS_TO_LONGS(nr) DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))

static inline unsigned bits_needed(uint32_t value)
{
    unsigned bits = 0;
    for (unsigned bit_test = 16; bit_test > 0; bit_test >>= 1) {
        if (value >> bit_test != 0) {
            bits += bit_test;
            value >>= bit_test;
        }
    }
    return bits + value;
}

static inline void bits_set(unsigned long* x, unsigned bit) { *x |= 1UL << bit; }
static inline bool bits_get(unsigned long* x, unsigned bit) { return !!((*x >> bit) & 1UL); }
static inline void bits_clr(unsigned long* x, unsigned bit) { *x &= ~(1UL << bit); }

#endif
