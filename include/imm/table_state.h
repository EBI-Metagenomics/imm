#ifndef IMM_TABLE_STATE_H_API
#define IMM_TABLE_STATE_H_API

#include "imm/api.h"

struct imm_abc;
struct imm_state;

IMM_API struct imm_state *imm_table_state_create(const char *name, const struct imm_abc *abc);
IMM_API void imm_table_state_destroy(struct imm_state *state);
IMM_API void imm_table_state_add(struct imm_state *state, const char *seq, double lprob);
IMM_API int imm_table_state_normalize(struct imm_state *state);

#endif
