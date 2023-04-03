#include "cspan.h"

struct imm_cspan imm_cspan_init(unsigned min, unsigned max)
{
  return (struct imm_cspan){.min = min, .max = max};
}

uint16_t imm_cspan_zip(struct imm_cspan const *x)
{
  return (uint16_t)((x->min << 8) | x->max);
}

bool imm_cspan_unzip(struct imm_cspan *x, uint16_t v)
{
  x->max = (uint8_t)v;
  x->min = (uint8_t)(v >> 8);
  return x->min <= x->max;
}
