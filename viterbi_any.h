#ifndef IMM_VITERBI_ANY_H
#define IMM_VITERBI_ANY_H

#include "compiler.h"
#include "remains.h"
#include "state_range.h"
#include "unsafe.h"
#include "viterbi.h"
#include "viterbi_best_incoming.h"
#include "viterbi_best_trans.h"
#include "viterbi_set_state_score.h"

IMM_TEMPLATE struct imm_ctrans const *
gany_state(struct imm_viterbi const *x, unsigned const row,
           struct state_range dst, struct imm_ctrans const *t,
           unsigned const seqlen, bool const save_path, bool const safe_future,
           bool const safe_past, bool const unsafe_state)
{
  float score = 0;
  struct viterbi_best_trans bt = {0};
  unsigned const remain = remains(seqlen, row, safe_future);

  t = viterbi_best_incoming(&score, &bt, x, row, dst.idx, t, save_path,
                            unsafe_state, safe_past);
  viterbi_set_state_score(x, row, dst, remain, score, &bt, save_path,
                          safe_future);
  return t;
}

IMM_TEMPLATE void gany_unsafe_state(struct imm_viterbi const *x,
                                    unsigned const row,
                                    struct unsafe const unsafe,
                                    unsigned const seqlen, bool const save_path,
                                    bool const safe_future,
                                    bool const safe_past)
{
  struct imm_ctrans const *t = x->dp->trans_table.trans + unsafe.trans;
  struct state_range dst = imm_viterbi_state_range(x, unsafe.state);
  gany_state(x, row, dst, t, seqlen, save_path, safe_future, safe_past, true);
}

IMM_TEMPLATE struct imm_ctrans const *
gany_safe_states(struct imm_ctrans const *t, struct imm_range const states,
                 struct imm_viterbi const *x, unsigned const row,
                 unsigned const seqlen, bool const save_path,
                 bool const safe_future, bool const safe_past)
{
  for (unsigned i = states.a; i < states.b; ++i)
  {
    struct state_range dst = imm_viterbi_state_range(x, i);
    t = gany_state(x, row, dst, t, seqlen, save_path, safe_future, safe_past,
                   false);
  }
  return t;
}

IMM_TEMPLATE void gany(struct imm_viterbi const *x, unsigned const row,
                       struct unsafe const unsafe, unsigned const seqlen,
                       bool const save_path, bool const safe_future,
                       bool const safe_past)
{
  gany_unsafe_state(x, row, unsafe, seqlen, save_path, safe_future, safe_past);
  struct imm_ctrans const *t = x->dp->trans_table.trans;
  struct imm_range const r = imm_range(0, x->dp->state_table.nstates);
  gany_safe_states(t, r, x, row, seqlen, save_path, safe_future, safe_past);
}

#endif
