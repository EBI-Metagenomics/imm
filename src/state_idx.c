#include "state_idx.h"
#include "free.h"
#include "uthash.h"

struct state_idx
{
    struct imm_state const* state;
    int                     idx;
    UT_hash_handle          hh;
};

void imm_state_idx_create(struct state_idx** head_ptr) { *head_ptr = NULL; }

void imm_state_idx_destroy(struct state_idx** head_ptr)
{
    struct state_idx *state_idx, *tmp;
    if (*head_ptr) {
        HASH_ITER(hh, *head_ptr, state_idx, tmp)
        {
            state_idx->state = NULL;
            state_idx->idx = -1;
            HASH_DEL(*head_ptr, state_idx);
            free_c(state_idx);
        }
    }
    *head_ptr = NULL;
}

void imm_state_idx_add(struct state_idx** head_ptr, struct imm_state const* state, int idx)
{
    struct state_idx* state_idx = malloc(sizeof(struct state_idx));
    state_idx->state = state;
    state_idx->idx = idx;
    HASH_ADD_PTR(*head_ptr, state, state_idx);
}

int imm_state_idx_find(struct state_idx const* head, struct imm_state const* state)
{
    struct state_idx* state_idx = NULL;
    HASH_FIND_PTR(head, &state, state_idx);
    return state_idx->idx;
}
