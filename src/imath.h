#ifndef IMATH_H
#define IMATH_H

#include "imm/report.h"
#include <limits.h>

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

#endif
