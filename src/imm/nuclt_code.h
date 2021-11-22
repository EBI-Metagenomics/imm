#ifndef IMM_NUCLT_CODE_H
#define IMM_NUCLT_CODE_H

#include "imm/code.h"
#include "imm/nuclt.h"

struct imm_nuclt;

struct imm_nuclt_code
{
    struct imm_code super;
    struct imm_nuclt const *nuclt;
};

static inline void imm_nuclt_code_init(struct imm_nuclt_code *code,
                                       struct imm_nuclt const *nuclt)
{
    imm_code_init(&code->super, &nuclt->super);
    code->nuclt = nuclt;
}

#endif
