#ifndef IMM_MM_STATE_H
#define IMM_MM_STATE_H

#include "src/imm/hide.h"

struct mm_state;
struct imm_state;

HIDE void mm_state_create(struct mm_state **head_ptr);
HIDE void mm_state_destroy(struct mm_state **head_ptr);

HIDE void mm_state_add_state(struct mm_state **head_ptr, int state_id,
                             const struct imm_state *state, double start_lprob);
HIDE int mm_state_del_state(struct mm_state **head_ptr, int state_id);

HIDE int mm_state_nitems(const struct mm_state *head);

HIDE struct mm_state *mm_state_find(struct mm_state *head, int state_id);
HIDE const struct mm_state *mm_state_find_c(const struct mm_state *head, int state_id);

HIDE const struct imm_state *mm_state_get_state(const struct mm_state *mm_state);
HIDE double mm_state_get_start_lprob(const struct mm_state *mm_state);
HIDE void mm_state_set_start_lprob(struct mm_state *mm_state, double lprob);

HIDE struct mm_state *mm_state_next(struct mm_state *mm_state);
HIDE const struct mm_state *mm_state_next_c(const struct mm_state *mm_state);

HIDE struct mm_trans *mm_state_get_trans(struct mm_state *mm_state);
HIDE const struct mm_trans *mm_state_get_trans_c(const struct mm_state *mm_state);

HIDE int mm_state_get_id(const struct mm_state *mm_state);

HIDE struct mm_trans **mm_state_get_trans_ptr(struct mm_state *mm_state);
HIDE struct mm_trans *const *mm_state_get_trans_ptr_c(const struct mm_state *mm_state);

#endif
