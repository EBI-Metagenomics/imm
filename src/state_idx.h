#ifndef IMM_STATE_IDX_H
#define IMM_STATE_IDX_H

#include "hide.h"

struct imm_state;
struct state_idx;

HIDE void imm_state_idx_create(struct state_idx** head_ptr);
HIDE void imm_state_idx_destroy(struct state_idx** head_ptr);
HIDE int  imm_state_idx_find(struct state_idx const* head, struct imm_state const* state);
HIDE void imm_state_idx_add(struct state_idx** head_ptr, struct imm_state const* state,
                            int idx);

#endif
