#include "state_idx.h"
#include "imm/imm.h"
#include "khash_ptr.h"
#include "log.h"
#include "std.h"

struct item
{
    struct imm_state const* state;
    uint_fast16_t           idx;
};

KHASH_MAP_INIT_PTR(item, struct item*)

struct state_idx
{
    struct item* items;
    struct item* curr_item;
    khash_t(item) * table;
};

void state_idx_add(struct state_idx* state_idx, struct imm_state const* state)
{
    int      ret = 0;
    khiter_t iter = kh_put(item, state_idx->table, state, &ret);
    BUG(ret == -1 || ret == 0);

    uint16_t     idx = (uint16_t)(state_idx->curr_item - state_idx->items);
    struct item* item = state_idx->curr_item++;
    item->state = state;
    item->idx = idx;

    kh_key(state_idx->table, iter) = item->state;
    kh_val(state_idx->table, iter) = item;
}

struct state_idx* state_idx_create(uint16_t size)
{
    struct state_idx* state_idx = xmalloc(sizeof(*state_idx));
    state_idx->items = malloc(sizeof(*state_idx->items) * size);
    if (!state_idx->items) {
        free(state_idx);
        error("%s", explain(IMM_OUTOFMEM));
        return NULL;
    }
    state_idx->curr_item = state_idx->items;
    state_idx->table = kh_init(item);
    return state_idx;
}

void state_idx_destroy(struct state_idx* state_idx)
{
    kh_destroy(item, state_idx->table);
    free(state_idx->items);
    free(state_idx);
}

uint_fast16_t state_idx_find(struct state_idx const* state_idx, struct imm_state const* state)
{
    khiter_t i = kh_get(item, state_idx->table, state);
    BUG(i == kh_end(state_idx->table));
    return (uint_fast16_t)kh_val(state_idx->table, i)->idx;
}
