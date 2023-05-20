#ifndef IMM_ZSPAN_H
#define IMM_ZSPAN_H

#include "assume.h"
#include "compiler.h"
#include "state.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

IMM_CONST IMM_TEMPLATE uint8_t imm_zspan(unsigned min, unsigned max)
{
  imm_assume(min <= IMM_STATE_MAX_SEQLEN);
  imm_assume(max <= IMM_STATE_MAX_SEQLEN);
  return (uint8_t)((min << 4) | max);
}

IMM_CONST IMM_TEMPLATE uint_fast8_t imm_zspan_min(uint8_t const x)
{
  return (uint_fast8_t)(x >> 4);
}

IMM_CONST IMM_TEMPLATE uint_fast8_t imm_zspan_max(uint8_t const x)
{
  return (uint_fast8_t)(x & 0xF);
}

#endif
