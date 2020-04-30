#ifndef IMM_DP_H
#define IMM_DP_H

#include "imm/export.h"

struct imm_dp;
struct imm_io;
struct imm_results;
struct imm_seq;
struct imm_state;

IMM_EXPORT struct imm_results const* imm_dp_viterbi(struct imm_dp const*  dp,
                                                    struct imm_seq const* seq,
                                                    unsigned              window_length);
IMM_EXPORT void                      imm_dp_destroy(struct imm_dp const* dp);

IMM_EXPORT void __imm_dp_create_from_io(struct imm_io* io);

#endif
