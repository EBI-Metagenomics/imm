#ifndef IMM_MM_STATE_H
#define IMM_MM_STATE_H

#include "hide.h"

struct mm_state;
struct imm_state;

HIDE void mm_state_create(struct mm_state** head_ptr);
HIDE void mm_state_destroy(struct mm_state** head_ptr);

HIDE void mm_state_add_state(struct mm_state** head_ptr, struct imm_state const* state,
                             double start_lprob);
HIDE int  mm_state_del_state(struct mm_state** head_ptr, struct imm_state const* state);
HIDE void mm_state_del_trans(struct mm_state* head, struct imm_state const* state);

HIDE int mm_state_nitems(struct mm_state const* head);

HIDE struct mm_state* mm_state_find(struct mm_state* head, struct imm_state const* state);
HIDE struct mm_state const* mm_state_find_c(struct mm_state const*  head,
                                            struct imm_state const* state);

HIDE struct imm_state const* mm_state_get_state(struct mm_state const* mm_state);
HIDE double                  mm_state_get_start_lprob(struct mm_state const* mm_state);
HIDE void mm_state_set_start_lprob(struct mm_state* mm_state, double lprob);

HIDE struct mm_state*       mm_state_next(struct mm_state* mm_state);
HIDE struct mm_state const* mm_state_next_c(struct mm_state const* mm_state);

HIDE struct mm_trans*       mm_state_get_trans(struct mm_state* mm_state);
HIDE struct mm_trans const* mm_state_get_trans_c(struct mm_state const* mm_state);

HIDE struct mm_trans**       mm_state_get_trans_ptr(struct mm_state* mm_state);
HIDE struct mm_trans* const* mm_state_get_trans_ptr_c(struct mm_state const* mm_state);

#endif
