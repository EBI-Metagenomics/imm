#ifndef IMM_STATE_IDX_H
#define IMM_STATE_IDX_H

#include "src/imm/hide.h"

struct imm_state;
struct state_idx;

HIDE void state_idx_create(struct state_idx** head_ptr);
HIDE void state_idx_destroy(struct state_idx** head_ptr);
HIDE int  state_idx_find(const struct state_idx* head, const struct imm_state* state);
HIDE void state_idx_add(struct state_idx** head_ptr, const struct imm_state* state, int idx);

#endif
