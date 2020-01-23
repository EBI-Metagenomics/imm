#include "mtrans_table.h"
#include "bug.h"
#include "free.h"
#include "imm/report.h"
#include "khash_ptr.h"
#include "mtrans.h"

KHASH_MAP_INIT_PTR(mtrans, struct mtrans*)

struct mtrans_table
{
    khash_t(mtrans) * ktable;
};

struct mtrans_table* mtrans_table_create(void)
{
    struct mtrans_table* table = malloc(sizeof(struct mtrans_table));
    table->ktable = kh_init(mtrans);
    return table;
}

void mtrans_table_destroy(struct mtrans_table* table)
{
    unsigned long i = 0;
    mtrans_table_for_each(i, table)
    {
        if (kh_exist(table->ktable, i))
            mtrans_destroy(kh_val(table->ktable, i));
    }

    kh_destroy(mtrans, table->ktable);
    free_c(table);
}

void mtrans_table_add(struct mtrans_table* table, struct mtrans* mtrans)
{
    int      ret = 0;
    khiter_t i = kh_put(mtrans, table->ktable, mtrans_get_state(mtrans), &ret);
    BUG(ret == -1 || ret == 0);
    kh_key(table->ktable, i) = mtrans_get_state(mtrans);
    kh_val(table->ktable, i) = mtrans;
}

void mtrans_table_del(struct mtrans_table* table, unsigned long iter)
{
    struct mtrans const* mtrans = mtrans_table_get(table, iter);
    mtrans_destroy(mtrans);
    kh_del(mtrans, table->ktable, (khiter_t)iter);
}

unsigned long mtrans_table_find(struct mtrans_table const* table,
                                struct imm_state const*    state)
{
    return kh_get(mtrans, table->ktable, state);
}

struct mtrans* mtrans_table_get(struct mtrans_table const* table, unsigned long iter)
{
    return kh_val(table->ktable, (khiter_t)iter);
}

int mtrans_table_exist(struct mtrans_table const* table, unsigned long iter)
{
    return kh_exist(table->ktable, (khiter_t)iter);
}

unsigned long mtrans_table_begin(struct mtrans_table const* table)
{
    return kh_begin(table->ktable);
}

unsigned long mtrans_table_end(struct mtrans_table const* table)
{
    return kh_end(table->ktable);
}

int mtrans_table_size(struct mtrans_table const* table)
{
    size_t n = kh_size(table->ktable);
    if (n > INT_MAX) {
        imm_error("size greater than INT_MAX");
        return -1;
    }

    return (int)n;
}
