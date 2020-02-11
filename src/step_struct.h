#ifndef IMM_STEP_STRUCT_H
#define IMM_STEP_STRUCT_H

#include "free.h"
#include "list.h"

struct imm_step
{
    struct imm_state const* state;
    unsigned                seq_len;
    struct list_head        list_entry;
};

#endif
