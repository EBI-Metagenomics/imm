#ifndef IMM_BITMAP_H
#define IMM_BITMAP_H

#include "assume.h"
#include "bits.h"
#include "compiler.h"
#include "minmax.h"
#include "unroll.h"

#define LONG_START(bit) (bit / BITS_PER_LONG)
#define BIT_START(bit) (bit % BITS_PER_LONG)

unsigned long *imm_bitmap_reallocf(unsigned long *, unsigned long bits);

CONST_ATTR TEMPLATE unsigned long __bitmap_get(unsigned long const *x,
                                               unsigned const addr,
                                               unsigned const nbits,
                                               unsigned long val)
{
  imm_assume(addr <= nbits);
  imm_assume(nbits <= 16);
  for (unsigned i = addr; i < addr + nbits; ++i)
  {
    if (bits_get(*x, i)) bits_set(&val, i - addr);
    else bits_clr(&val, i - addr);
  }
  return val;
}

PURE_ATTR TEMPLATE unsigned long imm_bitmap_get(unsigned long const *x,
                                                unsigned long const addr,
                                                unsigned const nbits)
{
  imm_assume(nbits <= 16);
  unsigned long const *y = x + LONG_START(addr);

  unsigned long val = 0;
  unsigned cut = MIN(nbits, BITS_PER_LONG - BIT_START(addr));
  val = __bitmap_get(y + 1, 0, nbits - cut, val) << cut;
  val = __bitmap_get(y, BIT_START(addr), cut, val);
  return val;
}

TEMPLATE void __bitmap_set(unsigned long *x, unsigned long const val,
                           unsigned const addr, unsigned const nbits)
{
  imm_assume(addr <= nbits);
  imm_assume(nbits <= 16);
  for (unsigned i = addr; i < addr + nbits; ++i)
  {
    if (bits_get(val, i - addr)) bits_set(x, i);
    else bits_clr(x, i);
  }
}

TEMPLATE void imm_bitmap_set(unsigned long *x, unsigned long val,
                             unsigned long const addr, unsigned const nbits)
{
  imm_assume(nbits <= 16);
  unsigned long *y = x + LONG_START(addr);

  unsigned cut = MIN(nbits, BITS_PER_LONG - BIT_START(addr));
  __bitmap_set(y, val, BIT_START(addr), cut);
  __bitmap_set(y + 1, val >> cut, 0, nbits - cut);
}

#endif
