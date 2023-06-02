#ifndef IMM_BITS_H
#define IMM_BITS_H

#include "compiler.h"
#include <stdbool.h>
#include <stdint.h>

#define __BITS_DIV_ROUND_UP(n, d) (((n) + (d)-1) / (d))

#define BITS_PER_BYTE 8
#define BITS_PER_LONG (sizeof(long) * BITS_PER_BYTE)
#define BITS_TO_LONGS(nr) __BITS_DIV_ROUND_UP(nr, BITS_PER_LONG)

imm_template void bits_clr(unsigned long *x, unsigned bit)
{
  *x &= ~(1UL << bit);
}

imm_const_template bool bits_get(unsigned long x, unsigned bit)
{
  return !!((x >> bit) & 1UL);
}

imm_template void bits_set(unsigned long *x, unsigned bit) { *x |= 1UL << bit; }

imm_const_template unsigned bits_width(uint32_t v)
{
  return v ? ((unsigned)__builtin_clz(v) ^ 31) + 1 : 0;
}

#endif
