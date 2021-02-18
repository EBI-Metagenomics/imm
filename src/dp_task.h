#ifndef DP_TASK_H
#define DP_TASK_H

#include <stdint.h>

struct dp_matrix;
struct eseq;
struct imm_seq;
struct imm_subseq;

struct imm_dp_task
{
    struct dp_matrix*     matrix;
    struct eseq*          eseq;
    struct imm_seq const* root_seq;
    uint16_t              window_length;
};

void dp_task_set_subseq(struct imm_dp_task* task, struct imm_subseq const* subseq);

#endif
