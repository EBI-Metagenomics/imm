#ifndef DP_BEST_TRANS_SCORE_H
#define DP_BEST_TRANS_SCORE_H

#include "imm/float.h"

struct imm_dp;
struct matrix;

imm_float best_trans_score_ge1(struct imm_dp const *, struct matrix const *,
                               unsigned dst, unsigned row);

imm_float best_trans_score(struct imm_dp const *, struct matrix const *,
                           unsigned dst, unsigned row);

imm_float best_trans_score_safe_ge1(struct imm_dp const *,
                                    struct matrix const *, unsigned dst,
                                    unsigned row);

imm_float best_trans_score_find_safe(struct imm_dp const *,
                                     struct matrix const *, unsigned dst,
                                     unsigned row);

imm_float best_trans_score_find_row0(struct imm_dp const *,
                                     struct matrix const *, unsigned dst);

#endif
