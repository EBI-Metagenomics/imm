#include "mstate_table.h"
#include "bug.h"
#include "free.h"
#include "imm/report.h"
#include "khash_ptr.h"
#include "mstate.h"
#include "mtrans.h"
#include "mtrans_table.h"

KHASH_MAP_INIT_PTR(mstate, struct mstate*)

struct mstate_table
{
    khash_t(mstate) * ktable;
};

struct mstate_table* mstate_table_create(void)
{
    struct mstate_table* mstate_table = malloc(sizeof(struct mstate_table));
    mstate_table->ktable = kh_init(mstate);
    return mstate_table;
}

void mstate_table_destroy(struct mstate_table* table)
{
    unsigned long i = 0;
    mstate_table_for_each(i, table)
    {
        if (kh_exist(table->ktable, i))
            mstate_destroy(kh_val(table->ktable, i));
    }

    kh_destroy(mstate, table->ktable);
    free_c(table);
}

void mstate_table_add(struct mstate_table* table, struct mstate* mstate)
{
    int      ret = 0;
    khiter_t i = kh_put(mstate, table->ktable, mstate_get_state(mstate), &ret);
    BUG(ret == -1 || ret == 0);
    kh_key(table->ktable, i) = mstate_get_state(mstate);
    kh_val(table->ktable, i) = mstate;
}

unsigned long mstate_table_find(struct mstate_table const* table,
                                struct imm_state const*    state)
{
    return kh_get(mstate, table->ktable, state);
}

struct mstate* mstate_table_get(struct mstate_table const* table, unsigned long iter)
{
    return kh_val(table->ktable, (khiter_t)iter);
}

int mstate_table_exist(struct mstate_table const* table, unsigned long iter)
{
    return kh_exist(table->ktable, (khiter_t)iter);
}

void mstate_table_del(struct mstate_table* table, unsigned long iter)
{
    struct mstate*          mstate = mstate_table_get(table, iter);
    struct imm_state const* state = mstate_get_state(mstate);

    mstate_destroy(mstate);
    kh_del(mstate, table->ktable, (khiter_t)iter);

    unsigned long i = 0;
    mstate_table_for_each(i, table)
    {
        if (mstate_table_exist(table, i)) {
            struct mtrans_table* tbl = mstate_get_mtrans_table(mstate_table_get(table, i));
            unsigned long        j = mtrans_table_find(tbl, state);
            if (j != mtrans_table_end(tbl))
                mtrans_table_del(tbl, j);
        }
    }
}

unsigned long mstate_table_begin(struct mstate_table const* table)
{
    return kh_begin(table->ktable);
}

unsigned long mstate_table_end(struct mstate_table const* table)
{
    return kh_end(table->ktable);
}

int mstate_table_size(struct mstate_table const* table)
{
    size_t n = kh_size(table->ktable);
    if (n > INT_MAX) {
        imm_error("size greater than INT_MAX");
        return -1;
    }

    return (int)n;
}

struct mstate const** mstate_table_array(struct mstate_table const* table)
{
    int                   size = mstate_table_size(table);
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
