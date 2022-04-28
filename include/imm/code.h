#ifndef IMM_CODE_H
#define IMM_CODE_H

#include "imm/export.h"
#include "imm/state_types.h"

struct imm_abc;

struct imm_code
{
    uint16_t offset[IMM_STATE_MAX_SEQLEN + 2];
    uint16_t stride[IMM_STATE_MAX_SEQLEN];
    struct imm_abc const *abc;
};

IMM_API void imm_code_init(struct imm_code *code, struct imm_abc const *abc);

#endif
