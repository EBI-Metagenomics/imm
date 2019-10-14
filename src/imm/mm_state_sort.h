#ifndef IMM_MM_STATE_SORT_H
#define IMM_MM_STATE_SORT_H

#include "src/imm/hide.h"

struct mm_state;

HIDE const struct mm_state **mm_state_sort(struct mm_state *const *head_ptr);

#endif
