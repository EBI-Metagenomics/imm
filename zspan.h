#ifndef IMM_ZSPAN_H
#define IMM_ZSPAN_H

#include "assume.h"
#include "compiler.h"
#include "range.h"
#include "state.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

IMM_CONST uint8_t imm_zspan(unsigned min, unsigned max)
{
  imm_assume(min <= IMM_STATE_MAX_SEQLEN);
  imm_assume(max <= IMM_STATE_MAX_SEQLEN);
  return (uint8_t)((min << 4) | max);
}

IMM_CONST uint_fast8_t imm_zspan_min(uint8_t x)
{
  return (uint_fast8_t)(x >> 4);
}

IMM_CONST uint_fast8_t imm_zspan_max(uint8_t x)
{
  return (uint_fast8_t)(x & 0xF);
}

IMM_CONST struct imm_range imm_zspan_range(uint8_t x)
{
  return imm_range(imm_zspan_min(x), imm_zspan_max(x) + 1);
}

#endif
