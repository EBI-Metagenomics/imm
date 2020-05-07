#ifndef IMATH_H
#define IMATH_H

#include "imm/report.h"
#include <inttypes.h>
#include <limits.h>

static inline unsigned long ipow(unsigned long base, unsigned exp);
static inline int           panic_sub_ui(unsigned a, unsigned b);
static inline int           panic_sub_ui32(uint32_t a, uint32_t b);

static inline unsigned long ipow(unsigned long base, unsigned exp)
{
    unsigned long result = 1;

    while (exp) {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        base *= base;
    }

    return result;
}

static inline int panic_sub_ui(unsigned a, unsigned b)
{
    if (a >= b) {

        unsigned r = a - b;
        if (r > INT_MAX)
            imm_die("panic_sub_ui overflow");
        return (int)r;
    }

    unsigned r = b - a;
    if (r > (unsigned)INT_MIN)
        imm_die("panic_sub_ui underflow");

    return (int)-r;
}

static inline int panic_sub_ui32(uint32_t a, uint32_t b)
{
    if (a >= b) {

        uint32_t r = a - b;
        if (r > INT_MAX)
            imm_die("panic_sub_ui32 overflow");
        return (int)r;
    }

    uint32_t r = b - a;
    if (r > (uint32_t)INT_MIN)
        imm_die("panic_sub_ui32 underflow");

    return (int)-r;
}

#endif
