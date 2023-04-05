#include "bitmap.h"
#include "bits.h"
#include "reallocf.h"
#include <stdlib.h>

#define LONG_START(bit) (bit / (sizeof(long) * BITS_PER_BYTE))
#define BIT_START(bit) (bit % (sizeof(long) * BITS_PER_BYTE))

unsigned long imm_bitmap_get(unsigned long *x, unsigned long start,
                             unsigned len)
{
  unsigned long val = 0;
  for (unsigned i = 0; i < len; ++i)
  {
    unsigned long j = start + (unsigned long)i;
    if (bits_get(x + LONG_START(j), BIT_START(j)))
      bits_set(&val, i);
    else
      bits_clr(&val, i);
  }
  return val;
}

unsigned long *imm_bitmap_reallocf(unsigned long *x, unsigned long bits)
{
  return imm_reallocf(x, sizeof(long) * BITS_TO_LONGS(bits));
}

void imm_bitmap_set(unsigned long *x, unsigned long val, unsigned long start,
                    unsigned len)
{
  for (unsigned i = 0; i < len; ++i)
  {
    unsigned long j = start + (unsigned long)i;
    if (bits_get(&val, i))
      bits_set(x + LONG_START(j), BIT_START(j));
    else
      bits_clr(x + LONG_START(j), BIT_START(j));
  }
}