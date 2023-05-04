#ifndef IMM_BITS_H
#define IMM_BITS_H

#include "compiler.h"
#include <stdbool.h>
#include <stdint.h>

#define __BITS_DIV_ROUND_UP(n, d) (((n) + (d)-1) / (d))

#define BITS_PER_BYTE 8
#define BITS_TO_LONGS(nr) __BITS_DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))

TEMPLATE void bits_clr(unsigned long *x, uint_fast8_t bit)
{
  *x &= ~(1UL << bit);
}

PURE_ATTR TEMPLATE bool bits_get(unsigned long const *x, uint_fast8_t bit)
{
  return !!((*x >> bit) & 1UL);
}

TEMPLATE void bits_set(unsigned long *x, uint_fast8_t bit) { *x |= 1UL << bit; }

CONST_ATTR TEMPLATE unsigned bits_width(uint32_t v)
{
  return v ? ((unsigned)__builtin_clz(v) ^ 31) + 1 : 0;
}

#endif
