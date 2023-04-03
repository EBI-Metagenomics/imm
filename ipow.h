#ifndef IMM_IPOW_H
#define IMM_IPOW_H

#include "compiler.h"

CONST_ATTR static unsigned long ipow(unsigned long x, unsigned e)
{
  return !e ? 1 : (e & 1 ? x : 1) * ipow(x * x, e >> 1);
}

#endif
