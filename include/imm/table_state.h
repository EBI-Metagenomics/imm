#ifndef IMM_TABLE_STATE_H
#define IMM_TABLE_STATE_H

#include "imm/export.h"

/** @file table_state.h
 * Table state module.
 *
 * An object of type @ref imm_table_state is meant to be immutable.
 */

struct imm_seq_table;
struct imm_state;
struct imm_table_state;

IMM_API struct imm_table_state const* imm_table_state_create(char const*                 name,
                                                             struct imm_seq_table const* table);
IMM_API struct imm_table_state const* imm_table_state_derived(struct imm_state const* state);
IMM_API void                          imm_table_state_destroy(struct imm_table_state const* state);
IMM_API struct imm_state const*       imm_table_state_super(struct imm_table_state const* state);

#endif
