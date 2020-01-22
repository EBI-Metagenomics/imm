#ifndef IMM_MSTATE_TABLE_H
#define IMM_MSTATE_TABLE_H

struct imm_state;
struct mstate;
/* TODO: check whether I can just rename khash_t(mstate) to struct mstate_table. */
struct mstate_table;

struct mstate_table* imm_mstate_table_create(void);
void                 imm_mstate_table_destroy(struct mstate_table* table);
void                 imm_mstate_table_add(struct mstate_table* table, struct mstate* mstate);
unsigned long        imm_mstate_table_find(struct mstate_table const* table,
                                           struct imm_state const*    state);
struct mstate* imm_mstate_table_get(struct mstate_table const* table, unsigned long iter);
int            imm_mstate_table_exist(struct mstate_table const* table, unsigned long iter);
void           imm_mstate_table_del(struct mstate_table* table, unsigned long iter);
unsigned long  imm_mstate_table_begin(struct mstate_table const* table);
unsigned long  imm_mstate_table_end(struct mstate_table const* table);
int            imm_mstate_table_nitems(struct mstate_table const* table);
struct mstate const** imm_mstate_table_array(struct mstate_table const* table);
/* TODO: rename nitems to size everywhere? */

/** iterate over buckets */
#define mstate_table_for_each(iter, tbl)                                                      \
    for (iter = imm_mstate_table_begin(tbl); iter < imm_mstate_table_end(tbl); ++iter)

#endif
