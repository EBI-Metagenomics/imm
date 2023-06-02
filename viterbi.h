#ifndef IMM_VITERBI_H
#define IMM_VITERBI_H

#include "assume.h"
#include "cell.h"
#include "compiler.h"
#include "dp.h"
#include "dp_safety.h"
#include "range.h"
#include "state_range.h"
#include "tardy_state.h"
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
  bool has_tardy_state;
  struct tardy_state tardy_state;
};

void imm_viterbi_init(struct imm_viterbi *, struct imm_dp const *,
                      struct imm_task *);
struct imm_range imm_viterbi_range(struct imm_viterbi const *, unsigned state);
float imm_viterbi_start_lprob(struct imm_viterbi const *);

TEMPLATE unsigned imm_viterbi_start_state(struct imm_viterbi const *x)
{
  return x->dp->state_table.start.state_idx;
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

TEMPLATE struct state_range imm_viterbi_state_range(struct imm_viterbi const *x,
                                                    unsigned const state)
{
  uint8_t span = imm_state_table_span(&x->dp->state_table, state);
  imm_assume(imm_zspan_min(span) <= IMM_STATE_MAX_SEQLEN);
  imm_assume(imm_zspan_max(span) <= IMM_STATE_MAX_SEQLEN);
  imm_assume(imm_zspan_min(span) <= imm_zspan_max(span));
  return STATE_RANGE(state, imm_zspan_min(span), imm_zspan_max(span));
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

TEMPLATE uint16_t imm_viterbi_trans_start(struct imm_viterbi const *x,
                                          unsigned state_idx)
{
  return imm_trans_table_trans_start(&x->dp->trans_table, state_idx);
}

#endif
