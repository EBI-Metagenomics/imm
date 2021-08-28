#ifndef SUPPORT_H
#define SUPPORT_H

#include <stdbool.h>
#include <stdlib.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define BITS_PER_BYTE 8
#define BITS_TO_LONGS(nr) DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))

#define DIV_ROUND_UP(n, d) (((n) + (d)-1) / (d))

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

static inline unsigned long ipow(unsigned long base, unsigned exp)
{
    unsigned long result = 1;

    while (exp)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        base *= base;
    }

    return result;
}

#ifndef HAVE_REALLOFC
void *reallocf(void *ptr, size_t size);
#endif

#endif
