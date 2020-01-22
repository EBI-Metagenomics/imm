#include "state_idx.h"
#include "free.h"
#include "imm/imm.h"
#include "khash_ptr.h"

struct item
{
    struct imm_state const* state;
    int                     idx;
};

KHASH_MAP_INIT_PTR(item, struct item*)

struct state_idx
{
    khash_t(item) * table;
};

struct state_idx* imm_state_idx_create(void)
{
    struct state_idx* state_idx = malloc(sizeof(struct state_idx));
    state_idx->table = kh_init(item);
    return state_idx;
}

void imm_state_idx_destroy(struct state_idx* state_idx)
{
    for (khiter_t i = kh_begin(state_idx->table); i < kh_end(state_idx->table); ++i) {
        if (kh_exist(state_idx->table, i))
            free_c(kh_val(state_idx->table, i));
    }

    kh_destroy(item, state_idx->table);
    free_c(state_idx);
}

void imm_state_idx_add(struct state_idx* state_idx, struct imm_state const* state, int idx)
{
    int      ret = 0;
    khiter_t iter = kh_put(item, state_idx->table, state, &ret);
    if (ret == -1)
        imm_die("hash table failed");
    if (ret == 0)
        imm_die("state already exist");

    struct item* item = malloc(sizeof(struct item));
    item->state = state;
    item->idx = idx;

    kh_key(state_idx->table, iter) = item->state;
    kh_val(state_idx->table, iter) = item;
}

int imm_state_idx_find(struct state_idx const* state_idx, struct imm_state const* state)
{
    khiter_t i = kh_get(item, state_idx->table, state);

    if (i == kh_end(state_idx->table))
        imm_die("state not found");

    return kh_val(state_idx->table, i)->idx;
}
