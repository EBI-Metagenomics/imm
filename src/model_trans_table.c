#include "model_trans_table.h"
#include "free.h"
#include "imm/bug.h"
#include "imm/report.h"
#include "imm/state.h"
#include "khash_ptr.h"
#include "model_trans.h"

KHASH_MAP_INIT_PTR(model_trans, struct model_trans*)

struct model_trans_table
{
    khash_t(model_trans) * ktable;
};

static void       model_trans_name_sort(struct model_trans const** model_trans, uint32_t ntrans);
static inline int name_compare(void const* a, void const* b);

void model_trans_table_add(struct model_trans_table* table, struct model_trans* mtrans)
{
    int      ret = 0;
    khiter_t i = kh_put(model_trans, table->ktable, model_trans_get_state(mtrans), &ret);
    IMM_BUG(ret == -1 || ret == 0);
    kh_key(table->ktable, i) = model_trans_get_state(mtrans);
    kh_val(table->ktable, i) = mtrans;
}

struct model_trans const** model_trans_table_array(struct model_trans_table const* table)
{
    uint32_t                   size = model_trans_table_size(table);
    struct model_trans const** mtrans = malloc(sizeof(*mtrans) * size);

    struct model_trans const** t = mtrans;
    unsigned long              i = 0;
    model_trans_table_for_each(i, table)
    {
        if (model_trans_table_exist(table, i)) {
            *t = model_trans_table_get(table, i);
            ++t;
        }
    }
    model_trans_name_sort(mtrans, model_trans_table_size(table));
    return mtrans;
}

unsigned long model_trans_table_begin(struct model_trans_table const* table)
{
    return kh_begin(table->ktable);
}

struct model_trans_table* model_trans_table_create(void)
{
    struct model_trans_table* table = malloc(sizeof(*table));
    table->ktable = kh_init(model_trans);
    return table;
}

void model_trans_table_del(struct model_trans_table* table, unsigned long iter)
{
    struct model_trans const* mtrans = model_trans_table_get(table, iter);
    model_trans_destroy(mtrans);
    kh_del(model_trans, table->ktable, (khiter_t)iter);
}

void model_trans_table_destroy(struct model_trans_table* table)
{
    unsigned long i = 0;
    model_trans_table_for_each(i, table)
    {
        if (kh_exist(table->ktable, i))
            model_trans_destroy(kh_val(table->ktable, i));
    }

    kh_destroy(model_trans, table->ktable);
    free_c(table);
}

unsigned long model_trans_table_end(struct model_trans_table const* table)
{
    return kh_end(table->ktable);
}

int model_trans_table_exist(struct model_trans_table const* table, unsigned long iter)
{
    return kh_exist(table->ktable, (khiter_t)iter);
}

unsigned long model_trans_table_find(struct model_trans_table const* table,
                                     struct imm_state const*         state)
{
    return kh_get(model_trans, table->ktable, state);
}

struct model_trans* model_trans_table_get(struct model_trans_table const* table, unsigned long iter)
{
    return kh_val(table->ktable, (khiter_t)iter);
}

unsigned model_trans_table_size(struct model_trans_table const* table)
{
    khint_t n = kh_size(table->ktable);
    IMM_BUG(n > UINT_MAX);
    return (unsigned)n;
}

static void model_trans_name_sort(struct model_trans const** mtrans, uint32_t ntrans)
{
    qsort(mtrans, ntrans, sizeof(*mtrans), name_compare);
}

static inline int name_compare(void const* a, void const* b)
{
    struct imm_state const* sa = model_trans_get_state(*(struct model_trans const**)a);
    struct imm_state const* sb = model_trans_get_state(*(struct model_trans const**)b);
    return strcmp(imm_state_get_name(sa), imm_state_get_name(sb));
}
