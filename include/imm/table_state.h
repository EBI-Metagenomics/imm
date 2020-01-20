#ifndef IMM_TABLE_STATE_H
#define IMM_TABLE_STATE_H

#include "imm/api.h"

struct imm_sequence_table;
struct imm_table_state;

IMM_API struct imm_table_state* imm_table_state_create(char const*                      name,
                                                       struct imm_sequence_table const* table);
IMM_API void                    imm_table_state_destroy(struct imm_table_state const* state);

#endif
