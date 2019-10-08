#ifndef IMM_STATE_IDX_H
#define IMM_STATE_IDX_H

struct state_idx;

void state_idx_create(struct state_idx **head_ptr);
void state_idx_destroy(struct state_idx **head_ptr);
int state_idx_find(const struct state_idx *head, int state_id);
void state_idx_add(struct state_idx **head_ptr, int state_id, int idx);

#endif
