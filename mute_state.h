#ifndef IMM_MUTE_STATE_H
#define IMM_MUTE_STATE_H

#include "export.h"
#include "state.h"

struct imm_abc;

struct imm_mute_state
{
  struct imm_state super;
};

IMM_API void imm_mute_state_init(struct imm_mute_state *, unsigned id,
                                 struct imm_abc const *);

#endif
