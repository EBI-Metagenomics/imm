#include "mstate_table.h"
#include "free.h"
#include "imm/imm.h"
#include "khash_ptr.h"
#include "mstate.h"
#include "mtrans.h"

KHASH_MAP_INIT_PTR(mstate, struct mstate*)

struct mstate_table
{
    khash_t(mstate) * table;
};

static khiter_t put_or_die(khash_t(mstate) * table, struct imm_state const* state);

struct mstate_table* mstate_table_create(void)
{
    struct mstate_table* mstate_table = malloc(sizeof(struct mstate_table));
    mstate_table->table = kh_init(mstate);
    return mstate_table;
}

void mstate_table_destroy(struct mstate_table* table)
{
    for (khiter_t i = kh_begin(table->table); i < kh_end(table->table); ++i) {
        if (kh_exist(table->table, i))
            mstate_destroy(kh_val(table->table, i));
    }
    kh_destroy(mstate, table->table);
    free_c(table);
}

void mstate_table_add(struct mstate_table* table, struct mstate* mstate)
{
    khiter_t i = put_or_die(table->table, mstate_get_state(mstate));
    kh_key(table->table, i) = mstate_get_state(mstate);
    kh_val(table->table, i) = mstate;
}

unsigned long mstate_table_find(struct mstate_table const* table,
                                    struct imm_state const*    state)
{
    return kh_get(mstate, table->table, state);
}

struct mstate* mstate_table_get(struct mstate_table const* table, unsigned long iter)
{
    return kh_val(table->table, (khiter_t)iter);
}

int mstate_table_exist(struct mstate_table const* table, unsigned long iter)
{
    return kh_exist(table->table, (khiter_t)iter);
}

void mstate_table_del(struct mstate_table* table, unsigned long iter)
{
    struct mstate*          mstate = mstate_table_get(table, iter);
    struct imm_state const* state = mstate_get_state(mstate);

    mstate_destroy(mstate);
    kh_del(mstate, table->table, (khiter_t)iter);

    unsigned long i = 0;
    mstate_table_for_each(i, table)
    {
        if (mstate_table_exist(table, i)) {
            mstate_del_trans(kh_val(table->table, i), state);
        }
    }
}

unsigned long mstate_table_begin(struct mstate_table const* table)
{
    return kh_begin(table->table);
}

unsigned long mstate_table_end(struct mstate_table const* table)
{
    return kh_end(table->table);
}

int mstate_table_nitems(struct mstate_table const* table)
{
    size_t n = kh_size(table->table);
    if (n > INT_MAX) {
        imm_error("nitems greater than INT_MAX");
        return -1;
    }

    return (int)n;
}

struct mstate const** mstate_table_array(struct mstate_table const* table)
{
    int                   size = mstate_table_nitems(table);
    struct mstate const** mstates = malloc(sizeof(struct mstate*) * (size_t)size);

    struct mstate const** mstate = mstates;
    unsigned long         i = 0;
    mstate_table_for_each(i, table)
    {
        if (mstate_table_exist(table, i)) {
            *mstate = mstate_table_get(table, i);
            ++mstate;
        }
    }
    return mstates;
}

static khiter_t put_or_die(khash_t(mstate) * table, struct imm_state const* state)
{
    int      ret = 0;
    khiter_t i = kh_put(mstate, table, state, &ret);
    if (ret == -1)
        imm_die("hash table failed");
    if (ret == 0)
        imm_die("state already exist");

    return i;
}
