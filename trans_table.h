#ifndef IMM_TRANS_TABLE_H
#define IMM_TRANS_TABLE_H

#include "assume.h"
#include "compiler.h"
#include "ctrans.h"
#include "state.h"
#include "trans.h"

struct imm_trans_table
{
  unsigned ntrans;
  struct imm_ctrans *trans;
  uint16_t *offset; /**< `trans[offset[dst]]` is the first transition and
                         `trans[offset[dst+1]-1]` is the last transition. */
};

struct imm_dp_cfg;

void imm_trans_table_init(struct imm_trans_table *);
int imm_trans_table_reset(struct imm_trans_table *, struct imm_dp_cfg const *);

unsigned imm_trans_table_idx(struct imm_trans_table const *, unsigned src,
                             unsigned dst);

void imm_trans_table_change(struct imm_trans_table *, unsigned trans,
                            float score);

void imm_trans_table_cleanup(struct imm_trans_table *);

unsigned imm_trans_table_transsize(unsigned ntrans);
unsigned imm_trans_table_offsize(unsigned nstates);

IMM_TEMPLATE unsigned imm_trans_table_ntrans(struct imm_trans_table const *tbl,
                                             unsigned const dst)
{
  imm_assume(tbl->offset[dst + 1] >= tbl->offset[dst]);
  return (unsigned)(tbl->offset[dst + 1] - tbl->offset[dst]);
}

IMM_TEMPLATE unsigned
imm_trans_table_source_state(struct imm_trans_table const *tbl,
                             unsigned const dst, unsigned const trans)
{
  return tbl->trans[tbl->offset[dst] + trans].src;
}

IMM_TEMPLATE float imm_trans_table_score(struct imm_trans_table const *tbl,
                                         unsigned const dst,
                                         unsigned const trans)
{
  return tbl->trans[tbl->offset[dst] + trans].score;
}

IMM_TEMPLATE uint16_t imm_trans_table_trans0(struct imm_trans_table const *tbl,
                                             unsigned const state)
{
  return tbl->offset[state];
}

#endif
