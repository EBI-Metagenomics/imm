#ifndef IMM_STATE_IDX_H
#define IMM_STATE_IDX_H

struct imm_state;
struct state_idx;

struct state_idx* state_idx_create(void);
void              state_idx_destroy(struct state_idx* state_idx);
void state_idx_add(struct state_idx* state_idx, struct imm_state const* state, unsigned idx);
unsigned state_idx_find(struct state_idx const* state_idx, struct imm_state const* state);

#endif
