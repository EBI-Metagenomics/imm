#ifndef IMM_HOT_RANGE_H
#define IMM_HOT_RANGE_H

#include <stdint.h>
#include <stdio.h>

struct imm_viterbi;

struct imm_hot_range
{
  unsigned start;
  unsigned end;
  uint8_t span;
  int pattern[2];
  uint8_t src_spans[2];
};

struct imm_hot_range imm_hot_range(struct imm_viterbi const *);
int imm_hot_range_code(struct imm_hot_range const *x);
void imm_hot_range_dump(struct imm_hot_range const *x, FILE *fp);

#endif
