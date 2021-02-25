#ifndef MODEL_TRANS_TABLE_H
#define MODEL_TRANS_TABLE_H

struct imm_state;
struct model_trans;
struct model_trans_table;

void                       model_trans_table_add(struct model_trans_table* table, struct model_trans* mtrans);
struct model_trans const** model_trans_table_array(struct model_trans_table const* table);
unsigned long              model_trans_table_begin(struct model_trans_table const* table);
struct model_trans_table*  model_trans_table_create(void);
void                       model_trans_table_del(struct model_trans_table* table, unsigned long iter);
void                       model_trans_table_destroy(struct model_trans_table* table);
unsigned long              model_trans_table_end(struct model_trans_table const* table);
int                        model_trans_table_exist(struct model_trans_table const* table, unsigned long iter);
unsigned long              model_trans_table_find(struct model_trans_table const* table, struct imm_state const* state);
struct model_trans*        model_trans_table_get(struct model_trans_table const* table, unsigned long iter);
unsigned                   model_trans_table_size(struct model_trans_table const* table);

/** iterate over buckets */
#define model_trans_table_for_each(iter, tbl)                                                                          \
    for (iter = model_trans_table_begin(tbl); iter < model_trans_table_end(tbl); ++iter)

#endif
