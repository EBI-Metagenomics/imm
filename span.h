#ifndef IMM_SPAN_H
#define IMM_SPAN_H

struct imm_span
{
  unsigned min;
  unsigned max;
};

static inline struct imm_span imm_span(unsigned min, unsigned max)
{
  return (struct imm_span){min, max};
}

#endif
