#ifndef IMM_STEP_H
#define IMM_STEP_H

#include "src/list.h"

struct imm_step
{
    struct imm_state const* state;
    int                     seq_len;
    struct list_head        list;
};

#endif
