#include "viterbi_generic.h"
#include "assume.h"
#include "compiler.h"
#include "cpath.h"
#include "hot_range.h"
#include "lprob.h"
#include "minmax.h"
#include "span.h"
#include "state.h"
#include "task.h"
#include "trans_table.h"
#include "viterbi.h"
#include "zspan.h"

struct bt
{
  unsigned prev_state;
  unsigned trans;
  unsigned len;
};

TEMPLATE void set_trans(struct bt *x, unsigned prev_state, unsigned trans,
                        unsigned len)
{
  x->prev_state = prev_state;
  x->trans = trans;
  x->len = len;
}

TEMPLATE void set_cpath(struct imm_cpath *path, struct bt const *bt, unsigned r,
                        unsigned i)
{
  if (bt->prev_state != IMM_STATE_NULL_IDX)
  {
    imm_cpath_set_trans(path, r, i, bt->trans);
    imm_cpath_set_seqlen(path, r, i, bt->len);
    assert(imm_cpath_trans(path, r, i) == bt->trans);
    assert(imm_cpath_seqlen(path, r, i) == bt->len);
  }
  else
  {
    imm_cpath_invalidate(path, r, i);
    assert(!imm_cpath_valid(path, r, i));
  }
}

TEMPLATE uint8_t state_span(struct imm_viterbi const *x, unsigned i)
{
  uint8_t span = imm_viterbi_span(x, i);
  imm_assume(imm_zspan_min(span) <= IMM_STATE_MAX_SEQLEN);
  imm_assume(imm_zspan_max(span) <= IMM_STATE_MAX_SEQLEN);
  imm_assume(imm_zspan_min(span) <= imm_zspan_max(span));
  return span;
}

TEMPLATE unsigned generic2_trans(struct imm_viterbi const *x, float *score,
                                 struct bt *bt, unsigned const row,
                                 unsigned const dst, unsigned const t0,
                                 bool const save_path, bool const unsafe_state,
                                 bool const safe_past)
{
  *score = imm_lprob_zero();
  *bt = (struct bt){IMM_STATE_NULL_IDX, IMM_STATE_NULL_SEQLEN, UINT16_MAX};

  unsigned t = t0;
  while (dst == x->dp->trans_table.trans[t].dst)
  {
    unsigned src = x->dp->trans_table.trans[t].src;
    uint8_t csrc = state_span(x, src);
    unsigned min = imm_zspan_min(csrc);
    unsigned max = imm_zspan_max(csrc);

    if (unsafe_state) min = MAX(min, 1U);
    if (!safe_past) max = MIN(max, row);

    imm_assume(max <= IMM_STATE_MAX_SEQLEN);
#pragma clang loop unroll(enable)
    for (unsigned len = min; len <= max; ++len)
    {
#pragma clang fp reassociate(on) contract(fast)
      float v0 = imm_viterbi_get_score(x, imm_cell(row - len, src, len));
      float v1 = x->dp->trans_table.trans[t].score;
      float v = v0 + v1;
      if (*score < v && save_path) set_trans(bt, src, t - t0, len - min);
      if (*score < v) *score = v;
    }
    ++t;
  }
  return t;
}

TEMPLATE unsigned hot_trans1100(struct imm_viterbi const *x, float *score,
                                struct bt *bt, unsigned const row,
                                unsigned const t0, bool const save_path,
                                bool const unsafe_state, bool const safe_past,
                                unsigned const src0, unsigned const src1)
{
  *score = imm_lprob_zero();
  *bt = (struct bt){IMM_STATE_NULL_IDX, IMM_STATE_NULL_SEQLEN, UINT16_MAX};

  unsigned const src[2] = {src0, src1};

  unsigned min[2] = {1, 0};
  unsigned max[2] = {1, 0};

  unsigned t = t0;
#pragma clang loop vectorize(enable)
  for (int i = 0; i < 2; ++i)
  {
    if (unsafe_state) min[i] = MAX(min[i], 1U);
    if (!safe_past) max[i] = MIN(max[i], row);

#pragma clang loop unroll(enable)
    for (unsigned len = min[i]; len <= max[i]; ++len)
    {
#pragma clang fp reassociate(on) contract(fast)
      float v0 = imm_viterbi_get_score(x, imm_cell(row - len, src[i], len));
      float v1 = x->dp->trans_table.trans[t].score;
      float v = v0 + v1;
      if (*score < v && save_path) set_trans(bt, src[i], t - t0, len - min[i]);
      if (*score < v) *score = v;
    }
    ++t;
  }

  return t;
}

TEMPLATE unsigned hot_trans0015(struct imm_viterbi const *x, float *score,
                                struct bt *bt, unsigned const row,
                                unsigned const t0, bool const save_path,
                                bool const unsafe_state, bool const safe_past,
                                unsigned const src0, unsigned const src1)
{
  *score = imm_lprob_zero();
  *bt = (struct bt){IMM_STATE_NULL_IDX, IMM_STATE_NULL_SEQLEN, UINT16_MAX};

  unsigned const src[2] = {src0, src1};

  unsigned min[2] = {0, 1};
  unsigned max[2] = {0, 5};

  unsigned t = t0;
#pragma clang loop vectorize(enable)
  for (int i = 0; i < 2; ++i)
  {
    if (unsafe_state) min[i] = MAX(min[i], 1U);
    if (!safe_past) max[i] = MIN(max[i], row);

#pragma clang loop unroll(enable)
    for (unsigned len = min[i]; len <= max[i]; ++len)
    {
#pragma clang fp reassociate(on) contract(fast)
      float v0 = imm_viterbi_get_score(x, imm_cell(row - len, src[i], len));
      float v1 = x->dp->trans_table.trans[t].score;
      float v = v0 + v1;
      if (*score < v && save_path) set_trans(bt, src[i], t - t0, len - min[i]);
      if (*score < v) *score = v;
    }
    ++t;
  }

  return t;
}

TEMPLATE void generic2_state(struct imm_viterbi const *x, float score,
                             struct bt const *bt, bool const row0,
                             unsigned const row, unsigned const dst,
                             unsigned const remain, bool const save_path,
                             bool const safe_future)
{
  if (row0 && imm_viterbi_start_state(x) == dst)
    score = MAX(imm_viterbi_start_lprob(x), score);

  if (save_path) set_cpath(&x->task->path, bt, row, dst);

  uint8_t cdst = state_span(x, dst);
  unsigned min = imm_zspan_min(cdst);
  unsigned max = imm_zspan_max(cdst);
  if (!safe_future) max = MIN(max, remain);

  imm_assume(max <= IMM_STATE_MAX_SEQLEN);
#pragma clang loop unroll(enable)
  for (unsigned len = min; len <= max; ++len)
  {
#pragma clang fp reassociate(on) contract(fast)
    float v = imm_viterbi_emission(x, row, dst, len, min);
    float total = score + v;
    imm_viterbi_set_score(x, imm_cell(row, dst, len), total);
  }
}

TEMPLATE void hot_state11(struct imm_viterbi const *x, float score,
                          struct bt const *bt, bool const row0,
                          unsigned const row, unsigned const dst,
                          unsigned const remain, bool const save_path,
                          bool const safe_future)
{
  if (row0 && imm_viterbi_start_state(x) == dst)
    score = MAX(imm_viterbi_start_lprob(x), score);

  if (save_path) set_cpath(&x->task->path, bt, row, dst);

  unsigned min = 1;
  unsigned max = 1;
  if (!safe_future) max = MIN(max, remain);

#pragma clang loop vectorize(enable)
  for (unsigned len = min; len <= max; ++len)
  {
#pragma clang fp reassociate(on) contract(fast)
    float v = imm_viterbi_emission(x, row, dst, len, min);
    float total = score + v;
    imm_viterbi_set_score(x, imm_cell(row, dst, len), total);
  }
}

TEMPLATE void hot_state15(struct imm_viterbi const *x, float score,
                          struct bt const *bt, bool const row0,
                          unsigned const row, unsigned const dst,
                          unsigned const remain, bool const save_path,
                          bool const safe_future)
{
  if (row0 && imm_viterbi_start_state(x) == dst)
    score = MAX(imm_viterbi_start_lprob(x), score);

  if (save_path) set_cpath(&x->task->path, bt, row, dst);

  unsigned min = 1;
  unsigned max = 5;
  if (!safe_future) max = MIN(max, remain);

#pragma clang loop vectorize(enable)
  for (unsigned len = min; len <= max; ++len)
  {
#pragma clang fp reassociate(on) contract(fast)
    float v = imm_viterbi_emission(x, row, dst, len, min);
    float total = score + v;
    imm_viterbi_set_score(x, imm_cell(row, dst, len), total);
  }
}

TEMPLATE void generic1(struct imm_viterbi const *x, bool const row0,
                       unsigned const row, unsigned const unsafe_state,
                       unsigned const unsafe_trans0, unsigned const remain,
                       bool const save_path, bool const safe_future,
                       bool const safe_past, struct imm_hot_range const *hot,
                       int const hot_code)
{
  float score = 0;
  struct bt bt = {0};

  generic2_trans(x, &score, &bt, row, unsafe_state, unsafe_trans0, save_path,
                 true, safe_past);
  generic2_state(x, score, &bt, row0, row, unsafe_state, remain, save_path,
                 safe_future);

  unsigned t = 0;

  if (hot_code)
  {
    for (unsigned i = 0; i < hot->start; ++i)
    {
      t = generic2_trans(x, &score, &bt, row, i, t, save_path, false,
                         safe_past);
      generic2_state(x, score, &bt, row0, row, i, remain, save_path,
                     safe_future);
    }

    unsigned srcs[2] = {x->dp->trans_table.trans[t].src,
                        x->dp->trans_table.trans[t + 1].src};
    for (unsigned i = hot->start; i < hot->end; ++i)
    {
      if (hot_code == 110011)
      {
        assert(imm_zspan_min(hot->src_spans[0]) == 1 &&
               imm_zspan_max(hot->src_spans[0]) == 1);
        assert(imm_zspan_min(hot->src_spans[1]) == 0 &&
               imm_zspan_max(hot->src_spans[1]) == 0);
        t = hot_trans1100(x, &score, &bt, row, t, save_path, false, safe_past,
                          srcs[0], srcs[1]);
        srcs[0] += hot->pattern[0];
        srcs[1] += hot->pattern[1];
        assert(imm_zspan_min(hot->span) == 1 && imm_zspan_max(hot->span) == 1);
        hot_state11(x, score, &bt, row0, row, i, remain, save_path,
                    safe_future);
      }
      else if (hot_code == 1515)
      {
        assert(imm_zspan_min(hot->src_spans[0]) == 0 &&
               imm_zspan_max(hot->src_spans[0]) == 0);
        assert(imm_zspan_min(hot->src_spans[1]) == 1 &&
               imm_zspan_max(hot->src_spans[1]) == 5);
        t = hot_trans0015(x, &score, &bt, row, t, save_path, false, safe_past,
                          srcs[0], srcs[1]);
        srcs[0] += hot->pattern[0];
        srcs[1] += hot->pattern[1];
        assert(imm_zspan_min(hot->span) == 1 && imm_zspan_max(hot->span) == 1);
        hot_state15(x, score, &bt, row0, row, i, remain, save_path,
                    safe_future);
      }
      else
      {
        fprintf(stderr, "Dont know about: %d\n", hot_code);
        assert(false);
      }
    }

    for (unsigned i = hot->end; i < x->dp->state_table.nstates; ++i)
    {
      t = generic2_trans(x, &score, &bt, row, i, t, save_path, false,
                         safe_past);
      generic2_state(x, score, &bt, row0, row, i, remain, save_path,
                     safe_future);
    }
  }
  else
  {
    for (unsigned i = 0; i < x->dp->state_table.nstates; ++i)
    {
      t = generic2_trans(x, &score, &bt, row, i, t, save_path, false,
                         safe_past);
      generic2_state(x, score, &bt, row0, row, i, remain, save_path,
                     safe_future);
    }
  }
}

CONST_ATTR TEMPLATE unsigned remains(unsigned const seqlen, unsigned const row,
                                     bool const safe_future)
{
  if (safe_future) return IMM_STATE_MAX_SEQLEN;
  if (seqlen >= IMM_STATE_MAX_SEQLEN + row) return IMM_STATE_MAX_SEQLEN;
  return seqlen - row;
}

TEMPLATE void generic0(struct imm_viterbi const *x, unsigned const unsafe_state,
                       unsigned const unsafe_state_trans0,
                       unsigned const seqlen, bool const save_path,
                       struct imm_hot_range const *hot, int const hot_code)
{
  struct imm_dp_safety const *y = &x->safety;
  unsigned const unsafe = unsafe_state_trans0;

  generic1(x, true, 0, unsafe_state, unsafe, remains(seqlen, 0, false),
           save_path, false, false, hot, hot_code);

  for (unsigned r = y->safe_future.a; r < y->safe_future.b; ++r)
    generic1(x, false, r, unsafe_state, unsafe, remains(seqlen, r, true),
             save_path, true, false, hot, hot_code);

  for (unsigned r = y->safe.a; r < y->safe.b; ++r)
    generic1(x, false, r, unsafe_state, unsafe, remains(seqlen, r, true),
             save_path, true, true, hot, hot_code);

  for (unsigned r = y->unsafe.a; r < y->unsafe.b; ++r)
    generic1(x, false, r, unsafe_state, unsafe, remains(seqlen, r, false),
             save_path, false, false, hot, hot_code);

  for (unsigned r = y->safe_past.a; r < y->safe_past.b; ++r)
    generic1(x, false, r, unsafe_state, unsafe, remains(seqlen, r, false),
             save_path, false, true, hot, hot_code);
}

void imm_viterbi_generic(struct imm_viterbi const *x, unsigned const seqlen)
{
  struct imm_hot_range hot = imm_hot_range(x);
  // imm_hot_range_dump(&hot, stdout);

  int hot_code = imm_hot_range_code(&hot);
  if (hot_code == 110011)
  {
    generic0(x, x->unsafe_state,
             imm_trans_table_trans0(&x->dp->trans_table, x->unsafe_state),
             seqlen, x->task->save_path, &hot, 110011);
  }
  else if (hot_code == 1515)
  {
    generic0(x, x->unsafe_state,
             imm_trans_table_trans0(&x->dp->trans_table, x->unsafe_state),
             seqlen, x->task->save_path, &hot, 1515);
  }
  else
  {
    generic0(x, x->unsafe_state,
             imm_trans_table_trans0(&x->dp->trans_table, x->unsafe_state),
             seqlen, x->task->save_path, &hot, 0);
  }
}
