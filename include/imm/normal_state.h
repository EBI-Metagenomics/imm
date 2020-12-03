#ifndef IMM_NORMAL_STATE_H
#define IMM_NORMAL_STATE_H

#include "imm/export.h"
#include <stdio.h>

/** @file normal_state.h
 * Normal state module.
 *
 * An object of type @ref imm_normal_state is meant to be immutable.
 */

struct imm_abc;
struct imm_hmm_block;
struct imm_state;

IMM_API struct imm_normal_state const* imm_normal_state_create(char const*           name,
                                                               struct imm_abc const* abc,
                                                               double const*         lprobs);
IMM_API struct imm_normal_state const* imm_normal_state_derived(struct imm_state const* state);
IMM_API void                    imm_normal_state_destroy(struct imm_normal_state const* state);
IMM_API struct imm_state const* imm_normal_state_read(FILE* stream, struct imm_abc const* abc);
IMM_API struct imm_state const* imm_normal_state_super(struct imm_normal_state const* state);
IMM_API int                     imm_normal_state_write(struct imm_state const*     state,
                                                       struct imm_hmm_block const* hmm_block, FILE* stream);

#endif
