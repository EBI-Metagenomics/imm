#include "viterbi_generic.h"
#include "hot_range.h"
#include "range.h"
#include "state_range.h"
#include "viterbi.h"
#include "viterbi_best_incoming.h"
#include "viterbi_best_trans.h"
#include "viterbi_set_state_score.h"

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

CONST_ATTR TEMPLATE unsigned remains(unsigned const seqlen, unsigned const row,
                                     bool const safe_future)
{
  if (safe_future) return IMM_STATE_MAX_SEQLEN;
  if (seqlen >= IMM_STATE_MAX_SEQLEN + row) return IMM_STATE_MAX_SEQLEN;
  return seqlen - row;
}

TEMPLATE struct imm_ctrans const *
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

  t = viterbi_best_incoming_1100(&score, &bt, x, row, src0, src1, t, save_path,
                                 unsafe_state, safe_past);
  if (row > 0 && safe_future && safe_past)
    viterbi_set_state_score_11_fast(x, row, dst, score, &bt, save_path);
  else
    viterbi_set_state_score(x, row, STATE_RANGE(dst, 1, 1), remain, score, &bt,
                            save_path, safe_future);
  return t;
}

TEMPLATE void gany_unsafe_state(struct imm_viterbi const *x, unsigned const row,
                                struct unsafe const unsafe,
                                unsigned const seqlen, bool const save_path,
                                bool const safe_future, bool const safe_past)
{
  struct imm_ctrans const *t = x->dp->trans_table.trans + unsafe.trans;
  struct state_range dst = imm_viterbi_state_range(x, unsafe.state);
  gany_state(x, row, dst, t, seqlen, save_path, safe_future, safe_past, true);
}

TEMPLATE struct imm_ctrans const *
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

TEMPLATE struct imm_ctrans const *
g1100_safe_states(struct imm_hot_range const *hot, struct imm_ctrans const *t,
                  struct imm_range const states, struct imm_viterbi const *x,
                  unsigned const row, unsigned const seqlen,
                  bool const save_path, bool const safe_future,
                  bool const safe_past)
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

TEMPLATE void gany(struct imm_viterbi const *x, unsigned const row,
                   struct unsafe const unsafe, unsigned const seqlen,
                   bool const save_path, bool const safe_future,
                   bool const safe_past)
{
  gany_unsafe_state(x, row, unsafe, seqlen, save_path, safe_future, safe_past);
  struct imm_ctrans const *t = x->dp->trans_table.trans;
  struct imm_range const r = imm_range(0, x->dp->state_table.nstates);
  gany_safe_states(t, r, x, row, seqlen, save_path, safe_future, safe_past);
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

static void viterbi_gany(struct imm_viterbi const *x, unsigned const seqlen,
                         struct unsafe unsafe, struct imm_dp_safety const *y,
                         bool const save_path)
{
  gany(x, 0, unsafe, seqlen, save_path, false, false);

  for (unsigned r = y->safe_future.a; r < y->safe_future.b; ++r)
    gany(x, r, unsafe, seqlen, save_path, true, false);

  for (unsigned r = y->safe.a; r < y->safe.b; ++r)
    gany(x, r, unsafe, seqlen, save_path, true, true);

  for (unsigned r = y->unsafe.a; r < y->unsafe.b; ++r)
    gany(x, r, unsafe, seqlen, save_path, false, false);

  for (unsigned r = y->safe_past.a; r < y->safe_past.b; ++r)
    gany(x, r, unsafe, seqlen, save_path, false, true);
}

static void viterbi_g1100(struct imm_hot_range hot, struct imm_viterbi const *x,
                          unsigned const seqlen, struct unsafe unsafe,
                          struct imm_dp_safety const *y, bool const save_path)
{
  g1100(&hot, x, 0, unsafe, seqlen, save_path, false, false);

  for (unsigned r = y->safe_future.a; r < y->safe_future.b; ++r)
    g1100(&hot, x, r, unsafe, seqlen, save_path, true, false);

  for (unsigned r = y->safe.a; r < y->safe.b; ++r)
    g1100(&hot, x, r, unsafe, seqlen, save_path, true, true);

  for (unsigned r = y->unsafe.a; r < y->unsafe.b; ++r)
    g1100(&hot, x, r, unsafe, seqlen, save_path, false, false);

  for (unsigned r = y->safe_past.a; r < y->safe_past.b; ++r)
    g1100(&hot, x, r, unsafe, seqlen, save_path, false, true);
}

void imm_viterbi_generic(struct imm_viterbi const *x, unsigned const seqlen)
{
  struct imm_hot_range hot = imm_hot_range(x);
  int hot_code = imm_hot_range_code(&hot);

  struct unsafe const unsafe = unsafe_init(x);
  struct imm_dp_safety const *y = &x->safety;
  bool const save_path = x->task->save_path;

  if (hot_code == 110011) viterbi_g1100(hot, x, seqlen, unsafe, y, save_path);
  else viterbi_gany(x, seqlen, unsafe, y, save_path);
}
