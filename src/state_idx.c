#include "state_idx.h"
#include "free.h"
#include "imm/imm.h"

struct state_idx
{
    struct imm_state const* state;
    int                     idx;
};

khash_t(state_idx) * imm_state_idx_create(void) { return kh_init(state_idx); }

void imm_state_idx_destroy(khash_t(state_idx) * table)
{
    for (khint_t i = kh_begin(table); i < kh_end(table); ++i) {
        if (kh_exist(table, i))
            free_c(kh_val(table, i));
    }

    kh_destroy(state_idx, table);
}

void imm_state_idx_add(khash_t(state_idx) * table, struct imm_state const* state, int idx)
{
    int     ret = 0;
    khint_t iter = kh_put(state_idx, table, state, &ret);
    if (ret == -1)
        imm_die("hash table failed");
    if (ret == 0)
        imm_die("state already exist");

    struct state_idx* state_idx = malloc(sizeof(struct state_idx));
    state_idx->state = state;
    state_idx->idx = idx;

    kh_key(table, iter) = state_idx->state;
    kh_val(table, iter) = state_idx;
}

int imm_state_idx_find(khash_t(state_idx) * table, struct imm_state const* state)
{
    khint_t i = kh_get(state_idx, table, state);

    if (i == kh_end(table))
        imm_die("could not find state");

    return kh_val(table, i)->idx;
}
