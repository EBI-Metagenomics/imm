#ifndef IMM_SEQUENCE_TABLE_H
#define IMM_SEQUENCE_TABLE_H

#include "imm/api.h"

struct imm_abc;
struct imm_sequence_table;

IMM_API struct imm_sequence_table* imm_sequence_table_create(struct imm_abc const* abc);
IMM_API void   imm_sequence_table_destroy(struct imm_sequence_table const* table);
IMM_API int    imm_sequence_table_add(struct imm_sequence_table* table, char const* seq,
                                      double lprob);
IMM_API int    imm_sequence_table_normalize(struct imm_sequence_table* table);
IMM_API double imm_sequence_table_lprob(struct imm_sequence_table const* table,
                                        char const* seq, int seq_len);
IMM_API struct imm_abc const* imm_sequence_table_get_abc(
    struct imm_sequence_table const* table);
IMM_API int imm_sequence_table_min_seq(struct imm_sequence_table const* table);
IMM_API int imm_sequence_table_max_seq(struct imm_sequence_table const* table);

#endif
