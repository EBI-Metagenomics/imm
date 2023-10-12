#ifndef IMM_IPOW_H
#define IMM_IPOW_H

#include "compiler.h"

IMM_CONST unsigned long imm_ipow(unsigned long x, unsigned e)
{
  return !e ? 1 : (e & 1 ? x : 1) * imm_ipow(x * x, e >> 1);
}

#endif
