#ifndef IMM_MM_TRANS_H
#define IMM_MM_TRANS_H

#include "src/imm/hide.h"

struct imm_state;
struct mm_trans;

HIDE void mm_trans_create(struct mm_trans** head_ptr);
HIDE void mm_trans_destroy(struct mm_trans** head_ptr);

HIDE struct mm_trans* mm_trans_add(struct mm_trans** head_ptr, const struct imm_state* state,
                                   double lprob);
HIDE void             mm_trans_del(struct mm_trans** head_ptr, struct mm_trans* trans);

HIDE struct mm_trans* mm_trans_find(struct mm_trans* head, const struct imm_state* state);
HIDE const struct mm_trans* mm_trans_find_c(const struct mm_trans*  head,
                                            const struct imm_state* state);

HIDE void   mm_trans_set_lprob(struct mm_trans* mm_trans, double lprob);
HIDE double mm_trans_get_lprob(const struct mm_trans* mm_trans);

HIDE struct mm_trans*       mm_trans_next(struct mm_trans* mm_trans);
HIDE const struct mm_trans* mm_trans_next_c(const struct mm_trans* mm_trans);

HIDE const struct imm_state* mm_trans_get_state(const struct mm_trans* mm_trans);

#endif
