#ifndef TASK_H
#define TASK_H

#include "dp/matrix.h"
#include "dp/path.h"
#include "eseq.h"
#include <stdbool.h>

struct imm_seq;

struct imm_task
{
    struct matrix matrix;
    struct path path;
    struct eseq eseq;
    struct imm_seq const *seq;
    bool save_path;
};

#endif
