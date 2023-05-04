#include "bitmap.h"
#include "bits.h"
#include "reallocf.h"
#include <stdlib.h>

unsigned long *imm_bitmap_reallocf(unsigned long *x, unsigned long bits)
{
  return imm_reallocf(x, sizeof(long) * BITS_TO_LONGS(bits));
}
