#ifndef IMM_VITERBI_H
#define IMM_VITERBI_H

#include "imm/float.h"

struct imm_dp;
struct imm_task;

struct imm_viterbi
{
    struct imm_dp const *dp;
    struct imm_task *task;
    unsigned unsafe_state;
};

imm_float imm_viterbi_score(struct imm_viterbi const *, unsigned dst,
                            unsigned row);
imm_float imm_viterbi_score_ge1(struct imm_viterbi const *, unsigned dst,
                                unsigned row);
imm_float imm_viterbi_score_safe(struct imm_viterbi const *, unsigned dst,
                                 unsigned row);
imm_float imm_viterbi_score_row0(struct imm_viterbi const *, unsigned dst);
imm_float imm_viterbi_score_safe_ge1(struct imm_viterbi const *, unsigned dst,
                                     unsigned row);

#endif
