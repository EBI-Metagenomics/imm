#include "mtrans_table.h"
#include "free.h"
#include "imm/bug.h"
#include "imm/report.h"
#include "imm/state.h"
#include "khash_ptr.h"
#include "mtrans.h"

KHASH_MAP_INIT_PTR(mtrans, struct mtrans*)

struct mtrans_table
{
    khash_t(mtrans) * ktable;
};

static void       mtrans_name_sort(struct mtrans const** mtrans, uint32_t ntrans);
static inline int name_compare(void const* a, void const* b);

void mtrans_table_add(struct mtrans_table* table, struct mtrans* mtrans)
{
    int      ret = 0;
    khiter_t i = kh_put(mtrans, table->ktable, mtrans_get_state(mtrans), &ret);
    IMM_BUG(ret == -1 || ret == 0);
    kh_key(table->ktable, i) = mtrans_get_state(mtrans);
    kh_val(table->ktable, i) = mtrans;
}

struct mtrans const** mtrans_table_array(struct mtrans_table const* table)
{
    uint32_t              size = mtrans_table_size(table);
    struct mtrans const** mtrans = malloc(sizeof(*mtrans) * size);

    struct mtrans const** t = mtrans;
    unsigned long         i = 0;
    mtrans_table_for_each (i, table) {
        if (mtrans_table_exist(table, i)) {
            *t = mtrans_table_get(table, i);
            ++t;
        }
    }
    mtrans_name_sort(mtrans, mtrans_table_size(table));
    return mtrans;
}

unsigned long mtrans_table_begin(struct mtrans_table const* table)
{
    return kh_begin(table->ktable);
}

struct mtrans_table* mtrans_table_create(void)
{
    struct mtrans_table* table = malloc(sizeof(*table));
    table->ktable = kh_init(mtrans);
    return table;
}

void mtrans_table_del(struct mtrans_table* table, unsigned long iter)
{
    struct mtrans const* mtrans = mtrans_table_get(table, iter);
    mtrans_destroy(mtrans);
    kh_del(mtrans, table->ktable, (khiter_t)iter);
}

void mtrans_table_destroy(struct mtrans_table* table)
{
    unsigned long i = 0;
    mtrans_table_for_each (i, table) {
        if (kh_exist(table->ktable, i))
            mtrans_destroy(kh_val(table->ktable, i));
    }

    kh_destroy(mtrans, table->ktable);
    free_c(table);
}

unsigned long mtrans_table_end(struct mtrans_table const* table) { return kh_end(table->ktable); }

int mtrans_table_exist(struct mtrans_table const* table, unsigned long iter)
{
    return kh_exist(table->ktable, (khiter_t)iter);
}

unsigned long mtrans_table_find(struct mtrans_table const* table, struct imm_state const* state)
{
    return kh_get(mtrans, table->ktable, state);
}

struct mtrans* mtrans_table_get(struct mtrans_table const* table, unsigned long iter)
{
    return kh_val(table->ktable, (khiter_t)iter);
}

unsigned mtrans_table_size(struct mtrans_table const* table)
{
    khint_t n = kh_size(table->ktable);
    IMM_BUG(n > UINT_MAX);
    return (unsigned)n;
}

static void mtrans_name_sort(struct mtrans const** mtrans, uint32_t ntrans)
{
    qsort(mtrans, ntrans, sizeof(*mtrans), name_compare);
}

static inline int name_compare(void const* a, void const* b)
{
    struct imm_state const* sa = mtrans_get_state(*(struct mtrans const**)a);
    struct imm_state const* sb = mtrans_get_state(*(struct mtrans const**)b);
    return strcmp(imm_state_get_name(sa), imm_state_get_name(sb));
}
