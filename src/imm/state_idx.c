#include "src/imm/state_idx.h"
#include "src/uthash/uthash.h"

struct state_idx
{
    const struct imm_state* state;
    int                     idx;
    UT_hash_handle          hh;
};

void state_idx_create(struct state_idx** head_ptr) { *head_ptr = NULL; }

void state_idx_destroy(struct state_idx** head_ptr)
{
    struct state_idx *state_idx, *tmp;
    if (*head_ptr) {
        HASH_ITER(hh, *head_ptr, state_idx, tmp)
        {
            state_idx->state = NULL;
            state_idx->idx = -1;
            HASH_DEL(*head_ptr, state_idx);
            free(state_idx);
        }
    }
    *head_ptr = NULL;
}

void state_idx_add(struct state_idx** head_ptr, const struct imm_state* state, int idx)
{
    struct state_idx* state_idx = malloc(sizeof(struct state_idx));
    state_idx->state = state;
    state_idx->idx = idx;
    HASH_ADD_PTR(*head_ptr, state, state_idx);
}

int state_idx_find(const struct state_idx* head, const struct imm_state* state)
{
    struct state_idx* state_idx = NULL;
    HASH_FIND_PTR(head, &state, state_idx);
    return state_idx->idx;
}
