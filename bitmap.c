#include "bitmap.h"
#include "bits.h"
#include "reallocf.h"

#define DIV_ROUND_UP(n, d) (((n) + (d)-1) / (d))
#define BITS_TYPE_COUNT(nr) DIV_ROUND_UP(nr, IMM_BITS_COUNT(long))

unsigned long *imm_bitmap_reallocf(unsigned long *x, unsigned long bits)
{
  return imm_reallocf(x, sizeof(long) * BITS_TYPE_COUNT(bits));
}
