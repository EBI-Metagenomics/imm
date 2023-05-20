#ifndef IMM_REMAINS_H
#define IMM_REMAINS_H

#include "compiler.h"
#include "state.h"

IMM_CONST IMM_TEMPLATE unsigned
remains(unsigned const seqlen, unsigned const row, bool const safe_future)
{
  if (safe_future) return IMM_STATE_MAX_SEQLEN;
  if (seqlen >= IMM_STATE_MAX_SEQLEN + row) return IMM_STATE_MAX_SEQLEN;
  return seqlen - row;
}

#endif
