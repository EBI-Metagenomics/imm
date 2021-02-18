#ifndef IPOW_H
#define IPOW_H

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
