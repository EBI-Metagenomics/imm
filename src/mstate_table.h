#ifndef MSTATE_TABLE_H
#define MSTATE_TABLE_H

#include <inttypes.h>

struct imm_state;
struct mstate;
struct mstate_table;

void                  mstate_table_add(struct mstate_table* table, struct mstate* mstate);
struct mstate const** mstate_table_array(struct mstate_table const* table);
unsigned long         mstate_table_begin(struct mstate_table const* table);
struct mstate_table*  mstate_table_create(void);
void                  mstate_table_del(struct mstate_table* table, unsigned long iter);
void                  mstate_table_destroy(struct mstate_table* table);
unsigned long         mstate_table_end(struct mstate_table const* table);
int                   mstate_table_exist(struct mstate_table const* table, unsigned long iter);
unsigned long  mstate_table_find(struct mstate_table const* table, struct imm_state const* state);
struct mstate* mstate_table_get(struct mstate_table const* table, unsigned long iter);
uint32_t       mstate_table_size(struct mstate_table const* table);

/** iterate over buckets */
#define mstate_table_for_each(iter, tbl)                                                           \
    for (iter = mstate_table_begin(tbl); iter < mstate_table_end(tbl); ++iter)

#endif
