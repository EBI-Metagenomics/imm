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

imm_pure_template unsigned
imm_viterbi_start_state_idx(struct imm_viterbi const *x)
{
  return x->dp->state_table.start.state_idx;
}

imm_pure_template unsigned imm_viterbi_ntrans(struct imm_viterbi const *x,
                                              unsigned dst)
{
  return imm_trans_table_ntrans(&x->dp->trans_table, dst);
}

imm_pure_template unsigned
imm_viterbi_source_state_idx(struct imm_viterbi const *x, unsigned dst,
                             unsigned trans)
{
  return imm_trans_table_source_state(&x->dp->trans_table, dst, trans);
}

imm_pure_template float imm_viterbi_get_score(struct imm_viterbi const *x,
                                              struct imm_cell cell)
{
  return imm_matrix_get_score(&x->task->matrix, cell);
}

imm_pure_template float imm_viterbi_trans_score(struct imm_viterbi const *x,
                                                unsigned dst, unsigned trans)
{
  return imm_trans_table_score(&x->dp->trans_table, dst, trans);
}

imm_pure_template uint8_t imm_viterbi_state_span(struct imm_viterbi const *x,
                                                 unsigned state_idx)
{
  return imm_state_table_zspan(&x->dp->state_table, state_idx);
}

imm_pure_template struct state_range
imm_viterbi_state_range(struct imm_viterbi const *x, unsigned state_idx)
{
  uint8_t span = imm_state_table_zspan(&x->dp->state_table, state_idx);
  imm_assume(imm_zspan_min(span) <= IMM_STATE_MAX_SEQLEN);
  imm_assume(imm_zspan_max(span) <= IMM_STATE_MAX_SEQLEN);
  imm_assume(imm_zspan_min(span) <= imm_zspan_max(span));
  return STATE_RANGE(state_idx, imm_zspan_min(span), imm_zspan_max(span));
}

imm_pure_template float imm_viterbi_emission(struct imm_viterbi const *x,
                                             unsigned row, unsigned state_idx,
                                             unsigned len, unsigned min_len)
{
  unsigned code = imm_eseq_get(&x->task->eseq, row, len, min_len);
  return imm_emis_score(&x->dp->emis, state_idx, code);
}

void imm_viterbi_generic(struct imm_viterbi const *);

#endif
