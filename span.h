#ifndef IMM_SPAN_H
#define IMM_SPAN_H

#include "compiler.h"

struct imm_span
{
  unsigned min;
  unsigned max;
};

imm_const_template struct imm_span imm_span(unsigned min, unsigned max)
{
  return (struct imm_span){min, max};
}

#endif
