#ifndef IMM_STATE_H
#define IMM_STATE_H

#include "imm/api.h"

/** @file state.h
 * State module.
 *
 * An object of type @ref imm_state is meant to be immutable.
 */

struct imm_abc;
struct imm_state;

typedef double (*imm_state_lprob_t)(struct imm_state const* state, char const* seq,
                                    int seq_len);
typedef int (*imm_state_min_seq_t)(struct imm_state const* state);
typedef int (*imm_state_max_seq_t)(struct imm_state const* state);

struct imm_state_funcs
{
    imm_state_lprob_t   lprob;
    imm_state_min_seq_t min_seq;
    imm_state_max_seq_t max_seq;
};

IMM_API struct imm_state const* imm_state_create(char const* name, struct imm_abc const* abc,
                                                 struct imm_state_funcs funcs, void* impl);
IMM_API void                    imm_state_destroy(struct imm_state const* state);

IMM_API char const* imm_state_get_name(struct imm_state const* state);

IMM_API struct imm_abc const* imm_state_get_abc(struct imm_state const* state);

IMM_API double imm_state_lprob(struct imm_state const* state, char const* seq, int seq_len);
IMM_API int    imm_state_min_seq(struct imm_state const* state);
IMM_API int    imm_state_max_seq(struct imm_state const* state);

IMM_API struct imm_state const* imm_state_cast_c(void const* state);

IMM_API void const* imm_state_get_impl_c(struct imm_state const* state);

#endif
