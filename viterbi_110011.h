#ifndef IMM_VITERBI_110011_H
#define IMM_VITERBI_110011_H

#include "compiler.h"
#include "hot_range.h"
#include "max.h"
#include "range.h"
#include "remains.h"
#include "unsafe.h"
#include "viterbi_any.h"
#include "viterbi_best_incoming.h"
#include "viterbi_best_trans.h"
#include "viterbi_set_state_score.h"

TEMPLATE struct imm_ctrans const *
g1100_state(unsigned const src0, unsigned const src1, unsigned const dst,
            struct imm_viterbi const *x, unsigned const row,
            struct imm_ctrans const *t, unsigned const seqlen,
            bool const save_path, bool const safe_future, bool const safe_past,
            bool const unsafe_state)
{
  float score = 0;
  struct viterbi_best_trans bt = {0};
  unsigned const remain = remains(seqlen, row, safe_future);

  if (row > 0 && safe_future && safe_past)
  {
    t = viterbi_best_incoming_1100_fast(&score, &bt, x, row, src0, src1, t,
                                        save_path);
    viterbi_set_state_score_11_fast(x, row, dst, score, &bt, save_path);
  }
  else
  {
    t = viterbi_best_incoming_1100(&score, &bt, x, row, src0, src1, t,
                                   save_path, unsafe_state, safe_past);
    viterbi_set_state_score(x, row, STATE_RANGE(dst, 1, 1), remain, score, &bt,
                            save_path, safe_future);
  }
  return t;
}

TEMPLATE struct imm_ctrans const *g1100_safe_states_fixed(
    struct imm_ctrans const *t, struct imm_range const states,
    struct imm_viterbi const *x, unsigned const row, unsigned const seqlen,
    bool const save_path, bool const safe_future, bool const safe_past)
{
  unsigned src0 = t[0].src;
  unsigned const src1 = t[1].src;
  for (unsigned dst = states.a; dst < states.b; ++dst)
  {
    t = g1100_state(src0, src1, dst, x, row, t, seqlen, save_path, safe_future,
                    safe_past, false);
    src0 += 1;
  }
  return t;
}

TEMPLATE struct imm_ctrans const *g1100_safe_states_unfixed(
    struct imm_hot_range const *hot, struct imm_ctrans const *t,
    struct imm_range const states, struct imm_viterbi const *x,
    unsigned const row, unsigned const seqlen, bool const save_path,
    bool const safe_future, bool const safe_past)
{
  unsigned src[2] = {t[0].src, t[1].src};
  for (unsigned dst = states.a; dst < states.b; ++dst)
  {
    t = g1100_state(src[0], src[1], dst, x, row, t, seqlen, save_path,
                    safe_future, safe_past, false);
    src[0] += hot->pattern[0];
    src[1] += hot->pattern[1];
  }
  return t;
}

TEMPLATE struct imm_ctrans const *
g1100_safe_states(struct imm_hot_range const *hot, struct imm_ctrans const *t,
                  struct imm_range const states, struct imm_viterbi const *x,
                  unsigned const row, unsigned const seqlen,
                  bool const save_path, bool const safe_future,
                  bool const safe_past)
{
  if (hot->pattern[0] == 1 && hot->pattern[1] == 0)
    return g1100_safe_states_fixed(t, states, x, row, seqlen, save_path,
                                   safe_future, safe_past);
  else
    return g1100_safe_states_unfixed(hot, t, states, x, row, seqlen, save_path,
                                     safe_future, safe_past);
}

TEMPLATE void g1100(struct imm_hot_range const *hot,
                    struct imm_viterbi const *x, unsigned const row,
                    struct unsafe const unsafe, unsigned const seqlen,
                    bool const save_path, bool const safe_future,
                    bool const safe_past)
{
  gany_unsafe_state(x, row, unsafe, seqlen, save_path, safe_future, safe_past);

  struct imm_ctrans const *t = x->dp->trans_table.trans;

  struct imm_range r = imm_range(0, hot->start);
  t = gany_safe_states(t, r, x, row, seqlen, save_path, safe_future, safe_past);

  r = imm_range(hot->start, hot->end);
  t = g1100_safe_states(hot, t, r, x, row, seqlen, save_path, safe_future,
                        safe_past);

  r = imm_range(hot->end, x->dp->state_table.nstates);
  t = gany_safe_states(t, r, x, row, seqlen, save_path, safe_future, safe_past);
}

TEMPLATE void
viterbi_g1100_template(struct imm_hot_range hot, struct imm_viterbi const *x,
                       unsigned const seqlen, struct unsafe unsafe,
                       struct imm_dp_safety const *y, bool const save_path)
{
  g1100(&hot, x, 0, unsafe, seqlen, save_path, false, false);

  for (unsigned r = imm_max(y->safe_future.a, 1U); r < y->safe_future.b; ++r)
    g1100(&hot, x, r, unsafe, seqlen, save_path, true, false);

  for (unsigned r = imm_max(y->safe.a, 1U); r < y->safe.b; ++r)
    g1100(&hot, x, r, unsafe, seqlen, save_path, true, true);

  for (unsigned r = imm_max(y->unsafe.a, 1U); r < y->unsafe.b; ++r)
    g1100(&hot, x, r, unsafe, seqlen, save_path, false, false);

  for (unsigned r = imm_max(y->safe_past.a, 1U); r < y->safe_past.b; ++r)
    g1100(&hot, x, r, unsafe, seqlen, save_path, false, true);
}

NOINLINE static void viterbi_g1100(struct imm_hot_range hot,
                                   struct imm_viterbi const *x,
                                   unsigned const seqlen, struct unsafe unsafe,
                                   struct imm_dp_safety const *y,
                                   bool const save_path)
{
  if (save_path) viterbi_g1100_template(hot, x, seqlen, unsafe, y, true);
  else viterbi_g1100_template(hot, x, seqlen, unsafe, y, false);
}

#endif
