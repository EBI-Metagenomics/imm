#ifndef IMM_VITERBI_H
#define IMM_VITERBI_H

#include "assume.h"
#include "cell.h"
#include "compiler.h"
#include "dp.h"
#include "dp_safety.h"
#include "range.h"
#include "task.h"
#include "trans_table.h"
#include "zspan.h"

struct imm_dp;
struct imm_task;

struct imm_viterbi
{
  struct imm_dp const *dp;
  struct imm_task *task;
  struct imm_dp_safety safety;
  unsigned seqlen;
  unsigned unsafe_state;
};

void imm_viterbi_init(struct imm_viterbi *, struct imm_dp const *,
                      struct imm_task *);
struct imm_range imm_viterbi_range(struct imm_viterbi const *, unsigned state);
float imm_viterbi_start_lprob(struct imm_viterbi const *);

TEMPLATE unsigned imm_viterbi_start_state(struct imm_viterbi const *x)
{
  return x->dp->state_table.start.state;
}

TEMPLATE unsigned imm_viterbi_ntrans(struct imm_viterbi const *x,
                                     unsigned const dst)
{
  return imm_trans_table_ntrans(&x->dp->trans_table, dst);
}

TEMPLATE unsigned imm_viterbi_source(struct imm_viterbi const *x,
                                     unsigned const dst, unsigned const trans)
{
  return imm_trans_table_source_state(&x->dp->trans_table, dst, trans);
}

TEMPLATE float imm_viterbi_get_score(struct imm_viterbi const *x,
                                     struct imm_cell const cell)
{
  return imm_matrix_get_score(&x->task->matrix, cell);
}

TEMPLATE float imm_viterbi_trans_score(struct imm_viterbi const *x,
                                       unsigned const dst, unsigned const trans)
{
  return imm_trans_table_score(&x->dp->trans_table, dst, trans);
}

TEMPLATE uint8_t imm_viterbi_span(struct imm_viterbi const *x,
                                  unsigned const state)
{
  return imm_state_table_span(&x->dp->state_table, state);
}

TEMPLATE float imm_viterbi_emission(struct imm_viterbi const *x,
                                    unsigned const row, unsigned const state,
                                    unsigned const len, unsigned const min_len)
{
  unsigned code = imm_eseq_get(&x->task->eseq, row, len, min_len);
  return imm_emis_score(&x->dp->emis, state, code);
}

TEMPLATE unsigned imm_viterbi_nstates(struct imm_viterbi const *x)
{
  return x->dp->state_table.nstates;
}

TEMPLATE void imm_viterbi_set_score(struct imm_viterbi const *x,
                                    struct imm_cell const cell,
                                    float const score)
{
  imm_matrix_set_score(&x->task->matrix, cell, score);
}

TEMPLATE uint16_t imm_viterbi_trans0(struct imm_viterbi const *x,
                                     unsigned const state)
{
  return imm_trans_table_trans0(&x->dp->trans_table, state);
}

#endif
