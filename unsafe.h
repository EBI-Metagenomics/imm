#ifndef IMM_UNSAFE_H
#define IMM_UNSAFE_H

#include "compiler.h"
#include "viterbi.h"

struct unsafe
{
  unsigned state;
  unsigned trans;
};

TEMPLATE struct unsafe unsafe_init(struct imm_viterbi const *x)
{
  unsigned t = imm_trans_table_trans0(&x->dp->trans_table, x->unsafe_state);
  return (struct unsafe){x->unsafe_state, t};
}

#endif
