#ifndef IMM_NUCLT_CODE_H
#define IMM_NUCLT_CODE_H

#include "code.h"
#include "compiler.h"
#include "nuclt.h"

struct imm_nuclt;

struct imm_nuclt_code
{
  struct imm_code super;
  struct imm_nuclt const *nuclt;
};

IMM_API extern struct imm_nuclt_code const imm_nuclt_code_empty;

static inline void imm_nuclt_code_init(struct imm_nuclt_code *code,
                                       struct imm_nuclt const *nuclt)
{
  imm_code_init(&code->super, &nuclt->super);
  code->nuclt = nuclt;
}

#endif
