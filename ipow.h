#ifndef IMM_IPOW_H
#define IMM_IPOW_H

#include "compiler.h"
#include <assert.h>

IMM_CONST_ATTRIBUTE long imm_ipow(int long x, int e)
{
  assert(e >= 0);
  return !e ? 1 : (e & 1 ? x : 1) * imm_ipow(x * x, e >> 1);
}

#endif
