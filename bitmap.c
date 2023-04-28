#include "bitmap.h"
#include "bits.h"
#include "reallocf.h"
#include <stdlib.h>

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
