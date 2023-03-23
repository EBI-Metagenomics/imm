#ifndef FINAL_SCORE_H
#define FINAL_SCORE_H

#include "imm/float.h"

struct final_score
{
    imm_float score;
    unsigned state;
    unsigned seq_len;
};

struct imm_dp;
struct imm_task;

struct final_score final_score(struct imm_dp const *, struct imm_task *);

#endif
