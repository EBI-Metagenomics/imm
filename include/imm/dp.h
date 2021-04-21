#ifndef IMM_DP_H
#define IMM_DP_H

#include "imm/export.h"
#include "imm/float.h"

struct imm_dp;
struct imm_dp_task;
struct imm_hmm;
struct imm_result;
struct imm_state;

IMM_API void                     imm_dp_destroy(struct imm_dp const* dp);
IMM_API struct imm_result const* imm_dp_viterbi(struct imm_dp const* dp, struct imm_dp_task* task);
IMM_API int imm_dp_change_trans(struct imm_dp* dp, struct imm_hmm* hmm, struct imm_state* src_state,
                                struct imm_state* tgt_state, imm_float lprob);

#endif
