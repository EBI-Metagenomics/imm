#ifndef IMM_SEQ_TABLE_H
#define IMM_SEQ_TABLE_H

#include "imm/api.h"
#include "imm/seq.h"

struct imm_abc;
struct imm_seq_table;

IMM_API struct imm_seq_table* imm_seq_table_create(struct imm_abc const* abc);
IMM_API struct imm_seq_table* imm_seq_table_clone(struct imm_seq_table const* table);
IMM_API void                  imm_seq_table_destroy(struct imm_seq_table const* table);
IMM_API int    imm_seq_table_add(struct imm_seq_table* table, struct imm_seq const* seq,
                                 double lprob);
IMM_API int    imm_seq_table_normalize(struct imm_seq_table* table);
IMM_API double imm_seq_table_lprob(struct imm_seq_table const* table,
                                   struct imm_seq const*       seq);
IMM_API struct imm_abc const* imm_seq_table_get_abc(struct imm_seq_table const* table);
IMM_API unsigned              imm_seq_table_min_seq(struct imm_seq_table const* table);
IMM_API unsigned              imm_seq_table_max_seq(struct imm_seq_table const* table);

#endif
