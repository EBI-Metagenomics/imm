#include "state_idx.h"
#include "imm/imm.h"
#include "khash_ptr.h"
#include "std.h"

struct item
{
    struct imm_state const* state;
    uint_fast16_t           idx;
};

KHASH_MAP_INIT_PTR(item, struct item*)

struct state_idx
{
    khash_t(item) * table;
};

void state_idx_add(struct state_idx* state_idx, struct imm_state const* state, uint_fast16_t idx)
{
    int      ret = 0;
    khiter_t iter = kh_put(item, state_idx->table, state, &ret);
    BUG(ret == -1 || ret == 0);

    struct item* item = malloc(sizeof(*item));
    item->state = state;
    item->idx = idx;

    kh_key(state_idx->table, iter) = item->state;
    kh_val(state_idx->table, iter) = item;
}

struct state_idx* state_idx_create(void)
{
    struct state_idx* state_idx = malloc(sizeof(*state_idx));
    state_idx->table = kh_init(item);
    return state_idx;
}

void state_idx_destroy(struct state_idx* state_idx)
{
    for (khiter_t i = kh_begin(state_idx->table); i < kh_end(state_idx->table); ++i) {
        if (kh_exist(state_idx->table, i))
            free(kh_val(state_idx->table, i));
    }

    kh_destroy(item, state_idx->table);
    free(state_idx);
}

uint_fast16_t state_idx_find(struct state_idx const* state_idx, struct imm_state const* state)
{
    khiter_t i = kh_get(item, state_idx->table, state);
    BUG(i == kh_end(state_idx->table));
    return (uint_fast16_t)kh_val(state_idx->table, i)->idx;
}
