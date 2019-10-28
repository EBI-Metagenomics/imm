#ifndef IMM_TABLE_STATE_H_API
#define IMM_TABLE_STATE_H_API

#include "imm/api.h"

struct imm_abc;
struct imm_table_state;

IMM_API struct imm_table_state* imm_table_state_create(char const*           name,
                                                       struct imm_abc const* abc);
IMM_API void                    imm_table_state_destroy(struct imm_table_state* state);
IMM_API void imm_table_state_add(struct imm_table_state* state, char const* seq,
                                 double lprob);
IMM_API int  imm_table_state_normalize(struct imm_table_state* state);

#endif
