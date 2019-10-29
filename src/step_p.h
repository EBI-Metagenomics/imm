#ifndef IMM_STATE_P_H
#define IMM_STATE_P_H

#include "list.h"

struct imm_step
{
    struct imm_state const* state;
    int                     seq_len;
    struct list_head        list;
};

#endif
