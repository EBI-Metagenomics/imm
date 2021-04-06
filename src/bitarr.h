#ifndef BITARR_H
#define BITARR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define BITS_PER_BYTE 8
#define DIV_ROUND_UP(n, d) (((n) + (d)-1) / (d))
#define BITS_TO_LONGS(nr) DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))

static inline void set_bit(unsigned long* x, unsigned bit) { *x |= 1UL << bit; }
static inline bool get_bit(unsigned long* x, unsigned bit) { return !!((*x >> bit) & 1UL); }
static inline void clr_bit(unsigned long* x, unsigned bit) { *x &= ~(1UL << bit); }

static inline void bitarr_set(unsigned long* x, size_t bit)
{
    set_bit(x + bit / (sizeof(*x) * BITS_PER_BYTE), bit % (sizeof(*x) * BITS_PER_BYTE));
}

static inline bool bitarr_get(unsigned long* x, size_t bit)
{
    return get_bit(x + bit / (sizeof(*x) * BITS_PER_BYTE), bit % (sizeof(*x) * BITS_PER_BYTE));
}

static inline void bitarr_clr(unsigned long* x, size_t bit)
{
    clr_bit(x + bit / (sizeof(*x) * BITS_PER_BYTE), bit % (sizeof(*x) * BITS_PER_BYTE));
}

static inline unsigned long* bitarr_alloc(size_t bits) { return malloc(sizeof(unsigned long) * BITS_TO_LONGS(bits)); }

#endif
