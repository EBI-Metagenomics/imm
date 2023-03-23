#ifndef TASK_H
#define TASK_H

#include "cpath.h"
#include "imm/eseq.h"
#include "imm/matrix.h"
#include <stdbool.h>

struct imm_seq;

struct imm_task
{
    struct imm_matrix matrix;
    struct cpath path;
    struct imm_eseq eseq;
    struct imm_seq const *seq;
    bool save_path;
};

#endif
