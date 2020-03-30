#ifndef IMM_DP_H
#define IMM_DP_H

#include "imm/export.h"

struct imm_dp;
struct imm_seq;
struct imm_state;
struct imm_results;

IMM_EXPORT struct imm_results const* imm_dp_viterbi(struct imm_dp const*    dp,
                                                    struct imm_seq const*   seq,
                                                    struct imm_state const* end_state,
                                                    unsigned                window_length);
IMM_EXPORT void                      imm_dp_destroy(struct imm_dp const* dp);

#endif
