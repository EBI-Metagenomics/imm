#include "model_state_table.h"
#include "bug.h"
#include "free.h"
#include "imm/imm.h"
#include "khash_ptr.h"
#include "model_state.h"
#include "model_state_sort.h"
#include "model_trans.h"
#include "model_trans_table.h"

KHASH_MAP_INIT_PTR(model_state, struct model_state*)

struct model_state_table
{
    khash_t(model_state) * ktable;
};

void model_state_table_add(struct model_state_table* table, struct model_state* mstate)
{
    int      ret = 0;
    khiter_t i = kh_put(model_state, table->ktable, model_state_get_state(mstate), &ret);
    BUG(ret == -1 || ret == 0);
    kh_key(table->ktable, i) = model_state_get_state(mstate);
    kh_val(table->ktable, i) = mstate;
}

struct model_state const** model_state_table_array(struct model_state_table const* table)
{
    uint32_t                   size = model_state_table_size(table);
    struct model_state const** mstates = malloc(sizeof(*mstates) * size);

    struct model_state const** mstate = mstates;
    unsigned long              i = 0;
    model_state_table_for_each(i, table)
    {
        if (model_state_table_exist(table, i)) {
            *mstate = model_state_table_get(table, i);
            ++mstate;
        }
    }
    model_state_name_sort(mstates, model_state_table_size(table));
    return mstates;
}

unsigned long model_state_table_begin(struct model_state_table const* table) { return kh_begin(table->ktable); }

struct model_state_table* model_state_table_create(void)
{
    struct model_state_table* mstate_table = malloc(sizeof(*mstate_table));
    mstate_table->ktable = kh_init(model_state);
    return mstate_table;
}

void model_state_table_del(struct model_state_table* table, unsigned long iter)
{
    struct model_state*     mstate = model_state_table_get(table, iter);
    struct imm_state const* state = model_state_get_state(mstate);

    model_state_destroy(mstate);
    kh_del(model_state, table->ktable, (khiter_t)iter);

    unsigned long i = 0;
    model_state_table_for_each(i, table)
    {
        if (model_state_table_exist(table, i)) {
            struct model_trans_table* tbl = model_state_get_mtrans_table(model_state_table_get(table, i));
            unsigned long             j = model_trans_table_find(tbl, state);
            if (j != model_trans_table_end(tbl))
                model_trans_table_del(tbl, j);
        }
    }
}

void model_state_table_destroy(struct model_state_table* table)
{
    unsigned long i = 0;
    model_state_table_for_each(i, table)
    {
        if (kh_exist(table->ktable, i))
            model_state_destroy(kh_val(table->ktable, i));
    }

    kh_destroy(model_state, table->ktable);
    free_c(table);
}

unsigned long model_state_table_end(struct model_state_table const* table) { return kh_end(table->ktable); }

int model_state_table_exist(struct model_state_table const* table, unsigned long iter)
{
    return kh_exist(table->ktable, (khiter_t)iter);
}

unsigned long model_state_table_find(struct model_state_table const* table, struct imm_state const* state)
{
    return kh_get(model_state, table->ktable, state);
}

struct model_state* model_state_table_get(struct model_state_table const* table, unsigned long iter)
{
    return kh_val(table->ktable, (khiter_t)iter);
}

uint32_t model_state_table_size(struct model_state_table const* table) { return (uint32_t)kh_size(table->ktable); }
