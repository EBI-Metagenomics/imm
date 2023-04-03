#ifndef IMM_CELL_H
#define IMM_CELL_H

#include "compiler.h"

struct imm_cell
{
  unsigned row;
  unsigned state;
  unsigned len;
};

CONST_ATTR TEMPLATE struct imm_cell imm_cell(unsigned row, unsigned state,
                                             unsigned len)
{
  return (struct imm_cell){.row = row, .state = state, .len = len};
}

#endif
