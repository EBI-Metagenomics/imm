#ifndef IPOW_H
#define IPOW_H

#include <assert.h>
#include <stdint.h>

static inline unsigned long ipow(unsigned long base, unsigned exp)
{
    unsigned long result = 1;

    while (exp)
    {
        if (exp & 1) result *= base;
        exp >>= 1;
        base *= base;
    }

    return result;
}

static inline uint16_t u16pow(unsigned base, unsigned exp)
{
    assert(base > 0);
    unsigned long r = ipow(base, exp);
    assert(r <= UINT16_MAX);
    return (uint16_t)r;
}

#endif
