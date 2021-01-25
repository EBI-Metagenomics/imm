#ifndef STEP_H
#define STEP_H

#include "free.h"
#include "list.h"

struct imm_step
{
    struct imm_state const* state;
    uint8_t                 seq_len;
    struct list_head        list_entry;
};

#endif
