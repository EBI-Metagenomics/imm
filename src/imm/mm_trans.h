#ifndef IMM_MM_TRANS_H
#define IMM_MM_TRANS_H

#include "src/imm/hide.h"

struct mm_trans;

HIDE void mm_trans_create(struct mm_trans **head_ptr);
HIDE void mm_trans_destroy(struct mm_trans **head_ptr);

HIDE struct mm_trans *mm_trans_add(struct mm_trans **head_ptr, int state_id, double lprob);

HIDE struct mm_trans *mm_trans_find(struct mm_trans *head, int state_id);
HIDE const struct mm_trans *mm_trans_find_c(const struct mm_trans *head, int state_id);

HIDE void mm_trans_set_lprob(struct mm_trans *mm_trans, double lprob);
HIDE double mm_trans_get_lprob(const struct mm_trans *mm_trans);

HIDE struct mm_trans *mm_trans_next(struct mm_trans *mm_trans);
HIDE const struct mm_trans *mm_trans_next_c(const struct mm_trans *mm_trans);

HIDE int mm_trans_get_id(const struct mm_trans *mm_trans);

#endif
