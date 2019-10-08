#ifndef IMM_MUTE_STATE_H_API
#define IMM_MUTE_STATE_H_API

#include "imm/api.h"

struct imm_abc;
struct imm_mute_state;
struct imm_state;

IMM_API struct imm_mute_state *imm_mute_state_create(const char *name,
                                                     const struct imm_abc *abc);
IMM_API void imm_mute_state_destroy(struct imm_mute_state *state);

IMM_API struct imm_state *imm_mute_state_cast(struct imm_mute_state *state);
IMM_API const struct imm_state *imm_mute_state_cast_c(const struct imm_mute_state *state);

#endif
