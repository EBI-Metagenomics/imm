#ifndef DP_VITERBI_H
#define DP_VITERBI_H

#include "imm/rc.h"

struct imm_dp;
struct imm_prod;
struct imm_task;

enum imm_rc viterbi(struct imm_dp const *dp, struct imm_task *task,
                    struct imm_prod *prod);

#endif
