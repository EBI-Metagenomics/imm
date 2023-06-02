#ifndef IMM_TRANS_TABLE_H
#define IMM_TRANS_TABLE_H

#include "assume.h"
#include "compiler.h"
#include "ctrans.h"
#include "state.h"
#include "trans.h"
#include <stdio.h>

struct imm_trans_table
{
  unsigned ntrans;
  struct imm_ctrans *trans;
  uint16_t *offset; /**< `trans[offset[dst]]` is the first transition and
                         `trans[offset[dst+1]-1]` is the last transition. */
};

struct imm_dp_cfg;
struct imm_state_table;

void imm_trans_table_init(struct imm_trans_table *);
int imm_trans_table_reset(struct imm_trans_table *, struct imm_dp_cfg const *);

unsigned imm_trans_table_idx(struct imm_trans_table const *, unsigned src,
                             unsigned dst);

void imm_trans_table_change(struct imm_trans_table *, unsigned trans,
                            float score);

void imm_trans_table_cleanup(struct imm_trans_table *);

unsigned imm_trans_table_transsize(unsigned ntrans);
unsigned imm_trans_table_offsize(unsigned nstates);

TEMPLATE unsigned imm_trans_table_ntrans(struct imm_trans_table const *tbl,
                                         unsigned const dst)
{
  imm_assume(tbl->offset[dst + 1] >= tbl->offset[dst]);
  return (unsigned)(tbl->offset[dst + 1] - tbl->offset[dst]);
}

TEMPLATE unsigned
imm_trans_table_source_state(struct imm_trans_table const *tbl,
                             unsigned const dst, unsigned const trans)
{
  return tbl->trans[tbl->offset[dst] + trans].src;
}

TEMPLATE float imm_trans_table_score(struct imm_trans_table const *tbl,
                                     unsigned const dst, unsigned const trans)
{
  return tbl->trans[tbl->offset[dst] + trans].score;
}

TEMPLATE uint16_t imm_trans_table_trans_start(struct imm_trans_table const *tbl,
                                              unsigned const state)
{
  return tbl->offset[state];
}

TEMPLATE struct imm_ctrans const *
imm_trans_table_ctrans_start(struct imm_trans_table const *x)
{
  return x->trans;
}

IMM_API void imm_trans_table_dump(struct imm_trans_table const *,
                                  struct imm_state_table const *st,
                                  imm_state_name *callb, FILE *restrict);

#endif
