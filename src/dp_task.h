#ifndef DP_TASK_H
#define DP_TASK_H

struct dp_matrix;
struct eseq;

struct imm_dp_task
{
    struct dp_matrix* matrix;
    struct eseq*      eseq;
};

#endif
