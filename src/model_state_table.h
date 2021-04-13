#ifndef MODEL_STATE_TABLE_H
#define MODEL_STATE_TABLE_H

#include <inttypes.h>

struct imm_state;
struct model_state;
struct model_state_table;

void                       model_state_table_add(struct model_state_table* table, struct model_state* mstate);
struct model_state const** model_state_table_array(struct model_state_table const* table);
unsigned long              model_state_table_begin(struct model_state_table const* table);
struct model_state_table*  model_state_table_create(void);
void                       model_state_table_del(struct model_state_table* table, unsigned long iter);
void                       model_state_table_destroy(struct model_state_table* table);
unsigned long              model_state_table_end(struct model_state_table const* table);
int                        model_state_table_exist(struct model_state_table const* table, unsigned long iter);
unsigned long              model_state_table_find(struct model_state_table const* table, struct imm_state const* state);
struct model_state*        model_state_table_get(struct model_state_table const* table, unsigned long iter);
uint16_t                   model_state_table_size(struct model_state_table const* table);

/** iterate over buckets */
#define model_state_table_for_each(iter, tbl)                                                                          \
    for (iter = model_state_table_begin(tbl); iter < model_state_table_end(tbl); ++iter)

#endif
