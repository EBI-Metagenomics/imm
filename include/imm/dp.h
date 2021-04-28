#ifndef IMM_DP_H
#define IMM_DP_H

#include "imm/export.h"
#include "imm/float.h"
#include "imm/state_types.h"

struct imm_dp;
struct imm_dp_task;
struct imm_hmm;
struct imm_result;
struct imm_state;

IMM_API void imm_dp_del(struct imm_dp const *dp);

IMM_API int imm_dp_change_trans(struct imm_dp *dp, imm_state_idx_t src,
                                imm_state_idx_t dst, imm_float lprob);

IMM_API struct imm_result const *imm_dp_viterbi(struct imm_dp const *dp,
                                                struct imm_dp_task *task);

#endif
