#ifndef IMM_TABLE_STATE_H_API
#define IMM_TABLE_STATE_H_API

#include "imm/api.h"

struct imm_abc;
struct imm_state;
struct imm_table_state;

IMM_API struct imm_table_state *imm_table_state_create(const char *name,
                                                       const struct imm_abc *abc);
IMM_API void imm_table_state_destroy(struct imm_table_state *state);

IMM_API struct imm_state *imm_table_state_cast(struct imm_table_state *state);
IMM_API const struct imm_state *imm_table_state_cast_c(const struct imm_table_state *state);

IMM_API void imm_table_state_add(struct imm_table_state *state, const char *seq,
                                 double lprob);

#endif
