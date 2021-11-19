#ifndef IMM_CODE_H
#define IMM_CODE_H

#include "imm/state_types.h"

struct imm_abc;

struct imm_code
{
    struct
    {
        unsigned min;
        unsigned max;
    } seqlen;
    uint16_t offset[IMM_STATE_MAX_SEQLEN + 1];
    uint16_t stride[IMM_STATE_MAX_SEQLEN];
    struct imm_abc const *abc;
};

#endif
