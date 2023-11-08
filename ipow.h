#ifndef IMM_IPOW_H
#define IMM_IPOW_H

#include "compiler.h"
#include <assert.h>

IMM_CONST long imm_ipow(long x, int e)
{
  assert(e >= 0);
  long value = 1;
  do
  {
    if (e & 1) value *= x;
    e >>= 1;
    x *= x;
  } while (e);
  return value;
}

#endif
