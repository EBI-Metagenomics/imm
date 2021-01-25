#ifndef IMM_DP_H
#define IMM_DP_H

#include "imm/export.h"
#include "imm/float.h"
#include <stdint.h>

struct imm_dp;
struct imm_dp_task;
struct imm_hmm;
struct imm_model;
struct imm_results;
struct imm_seq;
struct imm_state;

IMM_API void                      imm_dp_destroy(struct imm_dp const* dp);
IMM_API struct imm_results const* imm_dp_viterbi(struct imm_dp const* dp, struct imm_dp_task* task,
                                                 struct imm_seq const* seq, uint16_t window_length);
IMM_API int                       imm_dp_change_trans(struct imm_dp* dp, struct imm_hmm* hmm,
                                                      struct imm_state const* src_state,
                                                      struct imm_state const* tgt_state, imm_float lprob);

#endif
