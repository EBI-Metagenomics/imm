#ifndef IMM_STEP_H
#define IMM_STEP_H

#include "src/list.h"

struct imm_step
{
    const struct imm_state* state;
    int                     seq_len;
    struct list_head        list;
};

#endif
