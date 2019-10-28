#ifndef IMM_NORMAL_STATE_H_API
#define IMM_NORMAL_STATE_H_API

#include "imm/api.h"

struct imm_abc;
struct imm_state;

IMM_API struct imm_normal_state* imm_normal_state_create(char const*           name,
                                                         struct imm_abc const* abc,
                                                         double const*         lprobs);
IMM_API void                     imm_normal_state_destroy(struct imm_normal_state* state);
IMM_API int                      imm_normal_state_normalize(struct imm_normal_state* state);

#endif
