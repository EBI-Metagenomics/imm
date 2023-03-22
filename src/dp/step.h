#ifndef DP_STEP_H
#define DP_STEP_H

#include "imm/dp.h"
#include "imm/float.h"

struct imm_dp_step
{
    struct imm_dp_state_table const *state_table;
    struct imm_dp_trans_table const *trans_table;
    struct matrix *score;
};

static inline struct imm_dp_step imm_dp_step_init(struct imm_dp const *dp,
                                                  struct matrix *s)
{
    return (struct imm_dp_step){.state_table = &dp->state_table,
                                .trans_table = &dp->trans_table,
                                .score = s};
}

imm_float imm_dp_step_score(struct imm_dp_step const *, unsigned dst,
                            unsigned row);
imm_float imm_dp_step_score_ge1(struct imm_dp_step const *, unsigned dst,
                                unsigned row);
imm_float imm_dp_step_score_safe(struct imm_dp_step const *, unsigned dst,
                                 unsigned row);
imm_float imm_dp_step_score_row0(struct imm_dp_step const *, unsigned dst);
imm_float imm_dp_step_score_safe_ge1(struct imm_dp_step const *, unsigned dst,
                                     unsigned row);

#endif
