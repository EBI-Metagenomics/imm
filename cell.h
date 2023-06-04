#ifndef IMM_CELL_H
#define IMM_CELL_H

#include "compiler.h"
#include <stdint.h>

struct imm_cell
{
  unsigned row;
  uint_fast16_t state_idx;
  uint_fast8_t seqlen;
};

imm_const_template struct imm_cell imm_cell(unsigned const row,
                                            uint_fast16_t const state_idx,
                                            uint_fast8_t const seqlen)
{
  return (struct imm_cell){
      .row = row, .state_idx = state_idx, .seqlen = seqlen};
}

#endif
