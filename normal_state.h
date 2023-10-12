#ifndef IMM_NORMAL_STATE_H
#define IMM_NORMAL_STATE_H

#include "compiler.h"
#include "state.h"

struct imm_abc;

struct imm_normal_state
{
  struct imm_state super;
  float const *lprobs;
};

IMM_API void imm_normal_state_init(struct imm_normal_state *, unsigned id,
                                   struct imm_abc const *, float const *lprobs);

#endif
