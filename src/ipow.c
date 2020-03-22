#include "ipow.h"

unsigned long imm_ipow(unsigned long base, unsigned exp)
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
