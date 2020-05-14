#ifndef IMM_MUTE_STATE_H
#define IMM_MUTE_STATE_H

#include "imm/export.h"
#include <stdio.h>

/** @file mute_state.h
 * Mute state module.
 *
 * An object of type @ref imm_mute_state is meant to be immutable.
 */

struct imm_abc;
struct imm_mute_state;
struct imm_state;

IMM_API struct imm_mute_state const* imm_mute_state_create(char const*           name,
                                                           struct imm_abc const* abc);
IMM_API struct imm_mute_state const* imm_mute_state_derived(struct imm_state const* state);
IMM_API void                         imm_mute_state_destroy(struct imm_mute_state const* state);
IMM_API struct imm_state const*      imm_mute_state_read(FILE* stream, struct imm_abc const* abc);
IMM_API struct imm_state const*      imm_mute_state_super(struct imm_mute_state const* state);

#endif
