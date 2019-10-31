#ifndef IMM_MM_STATE_H
#define IMM_MM_STATE_H

#include "hide.h"

struct mstate;
struct imm_state;

HIDE void imm_mstate_create(struct mstate** head_ptr);
HIDE void imm_mstate_destroy(struct mstate** head_ptr);

HIDE void imm_mstate_add_state(struct mstate** head_ptr, struct imm_state const* state,
                               double start_lprob);
HIDE int  imm_mstate_del_state(struct mstate** head_ptr, struct imm_state const* state);
HIDE void imm_mstate_del_trans(struct mstate* head, struct imm_state const* state);

HIDE int imm_mstate_nitems(struct mstate const* head);

HIDE struct mstate*       imm_mstate_find(struct mstate* head, struct imm_state const* state);
HIDE struct mstate const* imm_mstate_find_c(struct mstate const*    head,
                                            struct imm_state const* state);

HIDE struct imm_state const* imm_mstate_get_state(struct mstate const* mm_state);
HIDE double                  imm_mstate_get_start(struct mstate const* mm_state);
HIDE void                    imm_mstate_set_start(struct mstate* mm_state, double lprob);

HIDE struct mstate*       imm_mstate_next(struct mstate* mm_state);
HIDE struct mstate const* imm_mstate_next_c(struct mstate const* mm_state);

HIDE struct mm_trans*       imm_mstate_get_trans(struct mstate* mm_state);
HIDE struct mm_trans const* imm_mstate_get_trans_c(struct mstate const* mm_state);

HIDE struct mm_trans**       imm_mstate_get_trans_ptr(struct mstate* mm_state);
HIDE struct mm_trans* const* imm_mstate_get_trans_ptr_c(struct mstate const* mm_state);

#endif
