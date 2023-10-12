#ifndef IMM_BITS_H
#define IMM_BITS_H

#include "compiler.h"
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

#define IMM_BITS_PER_BYTE CHAR_BIT
#define IMM_BITS_COUNT(T) (sizeof(T) * IMM_BITS_PER_BYTE)

IMM_INLINE void bits_clr(unsigned long *x, unsigned bit)
{
  *x &= ~(1UL << bit);
}

IMM_CONST bool bits_get(unsigned long x, unsigned bit)
{
  return !!((x >> bit) & 1UL);
}

IMM_INLINE void bits_set(unsigned long *x, unsigned bit) { *x |= 1UL << bit; }

IMM_CONST unsigned bits_width(uint32_t v)
{
  return v ? ((unsigned)__builtin_clz(v) ^ 31) + 1 : 0;
}

#endif
