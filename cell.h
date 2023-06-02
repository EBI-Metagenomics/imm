#ifndef IMM_CELL_H
#define IMM_CELL_H

#include "compiler.h"
#include <stdint.h>

struct imm_cell
{
  unsigned row;
  uint_fast16_t state;
  uint_fast8_t len;
};

imm_const_template struct imm_cell
imm_cell(unsigned const row, uint_fast16_t const state, uint_fast8_t const len)
{
  return (struct imm_cell){.row = row, .state = state, .len = len};
}

#endif
