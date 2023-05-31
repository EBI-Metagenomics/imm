#ifndef IMM_STATE_RANGE_H
#define IMM_STATE_RANGE_H

#include <stdint.h>

struct state_range
{
  uint_fast16_t idx;
  uint_fast8_t min;
  uint_fast8_t max;
};

#define STATE_RANGE(IDX, X, Y)                                                 \
  (struct state_range) { IDX, X, Y }

#endif
