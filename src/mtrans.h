#ifndef IMM_MTRANS_H
#define IMM_MTRANS_H

#include "hide.h"

struct imm_state;
struct mm_trans;

HIDE void imm_mtrans_create(struct mm_trans** head_ptr);
HIDE void imm_mtrans_destroy(struct mm_trans** head_ptr);

HIDE struct mm_trans* imm_mtrans_add(struct mm_trans**       head_ptr,
                                     struct imm_state const* state, double lprob);
HIDE void             imm_mtrans_del(struct mm_trans** head_ptr, struct mm_trans* trans);

HIDE struct mm_trans* imm_mtrans_find(struct mm_trans* head, struct imm_state const* state);
HIDE struct mm_trans const* imm_mtrans_find_c(struct mm_trans const*  head,
                                              struct imm_state const* state);

HIDE void   imm_mtrans_set_lprob(struct mm_trans* mm_trans, double lprob);
HIDE double imm_mtrans_get_lprob(struct mm_trans const* mm_trans);

HIDE struct mm_trans*       imm_mtrans_next(struct mm_trans* mm_trans);
HIDE struct mm_trans const* imm_mtrans_next_c(struct mm_trans const* mm_trans);

HIDE struct imm_state const* imm_mtrans_get_state(struct mm_trans const* mm_trans);

#endif
