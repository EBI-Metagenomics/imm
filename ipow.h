#ifndef IMM_IPOW_H
#define IMM_IPOW_H

#include "compiler.h"
#include <assert.h>

IMM_CONST long imm_ipow(long const x, int const e)
{
  assert(e >= 0);
  long r = 1;
  long xx = x;
  int ee = e;
  do
  {
    if (ee & 1) r *= xx;
    ee >>= 1;
    xx *= xx;
  } while (ee);
  return r;
}

#endif
