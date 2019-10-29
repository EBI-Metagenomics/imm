#ifndef IMM_MUTE_STATE_H
#define IMM_MUTE_STATE_H

#include "imm/api.h"

struct imm_abc;
struct imm_mute_state;

IMM_API struct imm_mute_state* imm_mute_state_create(char const*           name,
                                                     struct imm_abc const* abc);
IMM_API void                   imm_mute_state_destroy(struct imm_mute_state* state);

#endif
