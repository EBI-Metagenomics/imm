#ifndef DP_TASK_H
#define DP_TASK_H

#include "cpath.h"

struct dp_matrix;
struct eseq;
struct imm_seq;

struct imm_dp_task
{
    struct dp_matrix*     matrix;
    struct eseq*          eseq;
    struct cpath          cpath;
    struct imm_seq const* seq;
};

#endif
