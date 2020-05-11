#ifndef IMM_DP_H
#define IMM_DP_H

#include "imm/export.h"

struct imm_dp;
struct imm_model;
struct imm_results;
struct imm_seq;
struct imm_state;

IMM_API void                      imm_dp_destroy(struct imm_dp const* dp);
IMM_API struct imm_results const* imm_dp_viterbi(struct imm_dp const* dp, struct imm_seq const* seq,
                                                 unsigned window_length);

IMM_API void __imm_dp_create_from_io(struct imm_model* entry);

#endif
