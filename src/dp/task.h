#ifndef DP_TASK_H
#define DP_TASK_H

#include "dp/eseq.h"
#include "dp/matrix.h"
#include "dp/path.h"

struct imm_seq;

struct imm_dp_task
{
    struct matrix matrix;
    struct path path;
    struct eseq eseq;
    struct imm_seq const *seq;
};

#endif
