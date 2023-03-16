#ifndef DP_BEST_TRANS_H
#define DP_BEST_TRANS_H

#include "imm/float.h"

struct best_trans
{
    imm_float score;
    unsigned prev_state;
    unsigned prev_len;
    unsigned trans;
    unsigned len;
};

struct imm_dp;
struct matrix;

struct best_trans best_trans_find(struct imm_dp const *dp,
                                  struct matrix const *mt, unsigned dst,
                                  unsigned row);

struct best_trans best_trans_find_safe(struct imm_dp const *dp,
                                       struct matrix const *mt, unsigned dst,
                                       unsigned row);

struct best_trans best_trans_find_row0(struct imm_dp const *dp,
                                       struct matrix const *mt, unsigned dst);

#endif
