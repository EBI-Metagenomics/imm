#ifndef IMM_BITMAP_H
#define IMM_BITMAP_H

#include "bits.h"
#include "compiler.h"

#define LONG_START(bit) (bit / (sizeof(long) * BITS_PER_BYTE))
#define BIT_START(bit) (bit % (sizeof(long) * BITS_PER_BYTE))

unsigned long *imm_bitmap_reallocf(unsigned long *, unsigned long bits);

PURE_ATTR TEMPLATE unsigned long
imm_bitmap_get(unsigned long *x, unsigned long start, uint_fast8_t len)
{
  unsigned long val = 0;
  for (uint_fast8_t i = 0; i < len; ++i)
  {
    unsigned long j = start + (unsigned long)i;
    if (bits_get(x + LONG_START(j), BIT_START(j))) bits_set(&val, i);
    else bits_clr(&val, i);
  }
  return val;
}

TEMPLATE void imm_bitmap_set(unsigned long *x, unsigned long val,
                             unsigned long start, uint_fast8_t len)
{
  for (uint_fast8_t i = 0; i < len; ++i)
  {
    unsigned long j = start + (unsigned long)i;
    if (bits_get(&val, i)) bits_set(x + LONG_START(j), BIT_START(j));
    else bits_clr(x + LONG_START(j), BIT_START(j));
  }
}

#endif
