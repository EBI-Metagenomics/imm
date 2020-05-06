#ifndef MTRANS_TABLE_H
#define MTRANS_TABLE_H

struct imm_state;
struct mtrans;
struct mtrans_table;

void                 mtrans_table_add(struct mtrans_table* table, struct mtrans* mtrans);
unsigned long        mtrans_table_begin(struct mtrans_table const* table);
struct mtrans_table* mtrans_table_create(void);
void                 mtrans_table_del(struct mtrans_table* table, unsigned long iter);
void                 mtrans_table_destroy(struct mtrans_table* table);
unsigned long        mtrans_table_end(struct mtrans_table const* table);
int                  mtrans_table_exist(struct mtrans_table const* table, unsigned long iter);
unsigned long  mtrans_table_find(struct mtrans_table const* table, struct imm_state const* state);
struct mtrans* mtrans_table_get(struct mtrans_table const* table, unsigned long iter);
unsigned       mtrans_table_size(struct mtrans_table const* table);

/** iterate over buckets */
#define mtrans_table_for_each(iter, tbl)                                                           \
    for (iter = mtrans_table_begin(tbl); iter < mtrans_table_end(tbl); ++iter)

#endif
