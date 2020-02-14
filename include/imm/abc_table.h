#ifndef IMM_ABC_TABLE_H
#define IMM_ABC_TABLE_H

#include "imm/api.h"

struct imm_abc;
struct imm_abc_table;

IMM_API struct imm_abc_table const* imm_abc_table_create(struct imm_abc const* abc,
                                                         double const*         lprobs);
IMM_API double imm_abc_table_lprob(struct imm_abc_table const* abc_table, char symbol);
IMM_API void   imm_abc_table_destroy(struct imm_abc_table const* abc_table);
IMM_API struct imm_abc const* imm_abc_table_get_abc(struct imm_abc_table const* abc_table);

#endif
