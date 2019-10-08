#include "src/imm/state_idx.h"
#include "src/uthash/uthash.h"

struct state_idx
{
    int state_id;
    int idx;
    UT_hash_handle hh;
};

void state_idx_create(struct state_idx **head_ptr) { *head_ptr = NULL; }

void state_idx_destroy(struct state_idx **head_ptr)
{
    struct state_idx *state_idx, *tmp;
    if (*head_ptr) {
        HASH_ITER(hh, *head_ptr, state_idx, tmp)
        {
            state_idx->state_id = -1;
            state_idx->idx = -1;
            HASH_DEL(*head_ptr, state_idx);
            free(state_idx);
        }
    }
    *head_ptr = NULL;
}

void state_idx_add(struct state_idx **head_ptr, int state_id, int idx)
{
    struct state_idx *state_idx = malloc(sizeof(struct state_idx));
    state_idx->state_id = state_id;
    state_idx->idx = idx;
    HASH_ADD_INT(*head_ptr, state_id, state_idx);
}

int state_idx_find(const struct state_idx *head, int state_id)
{
    struct state_idx *state_idx = NULL;
    HASH_FIND_INT(head, &state_id, state_idx);
    return state_idx->idx;
}
