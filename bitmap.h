#ifndef IMM_BITMAP_H
#define IMM_BITMAP_H

#include "assume.h"
#include "bits.h"
#include "compiler.h"
#include "min.h"

#define IMM_BITMAP_LONG(bit) (bit / IMM_BITS_COUNT(long))
#define IMM_BITMAP_BIT(bit) (bit % IMM_BITS_COUNT(long))

unsigned long *imm_bitmap_reallocf(unsigned long *, unsigned long bits);

IMM_PURE unsigned long __bitmap_get(unsigned long const *x, unsigned addr,
                                    unsigned nbits, unsigned long val)
{
  imm_assume(addr + nbits <= IMM_BITS_COUNT(long));
  imm_assume(nbits <= 16);
  for (unsigned i = addr; i < addr + nbits; ++i)
  {
    if (bits_get(*x, i)) bits_set(&val, i - addr);
    else bits_clr(&val, i - addr);
  }
  return val;
}

IMM_PURE unsigned long imm_bitmap_get(unsigned long const *x,
                                      unsigned long const addr,
                                      unsigned const nbits)
{
  imm_assume(nbits <= 16);
  unsigned long const *y = x + IMM_BITMAP_LONG(addr);

  unsigned long val = 0;
  unsigned cut = imm_min(nbits, IMM_BITS_COUNT(long) - IMM_BITMAP_BIT(addr));
  val = __bitmap_get(y + 1, 0, nbits - cut, val) << cut;
  val = __bitmap_get(y, IMM_BITMAP_BIT(addr), cut, val);
  return val;
}

IMM_INLINE void __bitmap_set(unsigned long *x, unsigned long val, unsigned addr,
                             unsigned nbits)
{
  imm_assume(addr + nbits <= IMM_BITS_COUNT(long));
  imm_assume(nbits <= 16);
  for (unsigned i = addr; i < addr + nbits; ++i)
  {
    if (bits_get(val, i - addr)) bits_set(x, i);
    else bits_clr(x, i);
  }
}

IMM_INLINE void imm_bitmap_set(unsigned long *x, unsigned long val,
                               unsigned long addr, unsigned nbits)
{
  imm_assume(nbits <= 16);
  unsigned long *y = x + IMM_BITMAP_LONG(addr);

  unsigned cut = imm_min(nbits, IMM_BITS_COUNT(long) - IMM_BITMAP_BIT(addr));
  __bitmap_set(y, val, IMM_BITMAP_BIT(addr), cut);
  __bitmap_set(y + 1, val >> cut, 0, nbits - cut);
}

#endif
