#ifndef STEP_H
#define STEP_H

#include "list.h"
#include <stdint.h>

struct imm_state;

struct imm_step
{
    struct imm_state const* state;
    uint8_t                 seq_len;
    struct list_head        list_entry;
};

#endif
