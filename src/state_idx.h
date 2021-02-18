#ifndef STATE_IDX_H
#define STATE_IDX_H

#include <inttypes.h>

struct imm_state;
struct state_idx;

void              state_idx_add(struct state_idx* state_idx, struct imm_state const* state, uint_fast16_t idx);
struct state_idx* state_idx_create(void);
void              state_idx_destroy(struct state_idx* state_idx);
uint_fast16_t     state_idx_find(struct state_idx const* state_idx, struct imm_state const* state);

#endif
