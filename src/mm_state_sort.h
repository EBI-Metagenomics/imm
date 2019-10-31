#ifndef IMM_MM_STATE_SORT_H
#define IMM_MM_STATE_SORT_H

#include "hide.h"

struct mstate;

HIDE struct mstate const* const* imm_mstate_sort(struct mstate const* mm_state_head);

#endif
