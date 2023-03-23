#ifndef BEST_TRANS_H
#define BEST_TRANS_H

#include "imm/float.h"
#include "imm/lprob.h"
#include "imm/state_types.h"

struct best_trans
{
    imm_float score;
    unsigned prev_state;
    unsigned trans;
    unsigned len;
};

struct imm_dp;
struct imm_matrix;

static inline struct best_trans best_trans_init(void)
{
    return (struct best_trans){imm_lprob_zero(), IMM_STATE_NULL_IDX,
                               IMM_STATE_NULL_SEQLEN, UINT16_MAX};
}

struct best_trans best_trans_find_ge1(struct imm_dp const *,
                                      struct imm_matrix const *, unsigned dst,
                                      unsigned row);

struct best_trans best_trans_find(struct imm_dp const *,
                                  struct imm_matrix const *, unsigned dst,
                                  unsigned row);

struct best_trans best_trans_find_safe_ge1(struct imm_dp const *,
                                           struct imm_matrix const *,
                                           unsigned dst, unsigned row);

struct best_trans best_trans_find_safe(struct imm_dp const *,
                                       struct imm_matrix const *, unsigned dst,
                                       unsigned row);

struct best_trans best_trans_find_row0(struct imm_dp const *,
                                       struct imm_matrix const *, unsigned dst);

#endif
