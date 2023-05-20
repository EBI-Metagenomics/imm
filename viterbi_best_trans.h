#ifndef IMM_VITERBI_BEST_TRANS_H
#define IMM_VITERBI_BEST_TRANS_H

#include "compiler.h"
#include "state.h"
#include "trans.h"
#include <stdint.h>

struct viterbi_best_trans
{
  uint_fast16_t src_idx;
  uint_fast16_t src_trans;
  uint_fast8_t src_seqlen;
};

IMM_TEMPLATE void viterbi_best_trans_set(struct viterbi_best_trans *x,
                                         uint_fast16_t const src,
                                         uint_fast16_t const trans,
                                         uint_fast8_t const len)
{
  x->src_idx = src;
  x->src_trans = trans;
  x->src_seqlen = len;
}

#define VITERBI_BEST_TRANS_INIT                                                \
  {                                                                            \
    IMM_STATE_NULL_IDX, IMM_TRANS_NULL_IDX, IMM_STATE_NULL_SEQLEN              \
  }

#define VITERBI_BEST_TRANS_DECLARE(NAME)                                       \
  struct viterbi_best_trans NAME = VITERBI_BEST_TRANS_INIT

#endif
