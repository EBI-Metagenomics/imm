#ifndef IMM_STATE_IDX_H
#define IMM_STATE_IDX_H

#include "hide.h"

struct imm_state;
struct state_idx;

HIDE struct state_idx* imm_state_idx_create(void);
HIDE void              imm_state_idx_destroy(struct state_idx* state_idx);
HIDE void imm_state_idx_add(struct state_idx* state_idx, struct imm_state const* state,
                            int idx);
HIDE int  imm_state_idx_find(struct state_idx const* state_idx, struct imm_state const* state);

#endif
