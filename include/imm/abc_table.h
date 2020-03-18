#ifndef IMM_ABC_TABLE_H
#define IMM_ABC_TABLE_H

#include "imm/abc.h"
#include "imm/api.h"
#include "imm/lprob.h"
#include "imm/report.h"

struct imm_abc_table
{
    struct imm_abc const* abc;
    double                lprobs[];
};

IMM_API struct imm_abc_table const* imm_abc_table_create(struct imm_abc const* abc,
                                                         double const*         lprobs);
static inline double imm_abc_table_lprob(struct imm_abc_table const* abc_table, char symbol)
{
    int idx = imm_abc_symbol_idx(abc_table->abc, symbol);
    if (idx < 0) {
        imm_error("symbol not found");
        return imm_lprob_invalid();
    }
    size_t i = (size_t)idx;
    return abc_table->lprobs[i];
}
IMM_API void imm_abc_table_destroy(struct imm_abc_table const* abc_table);
static inline struct imm_abc const* imm_abc_table_get_abc(
    struct imm_abc_table const* abc_table)
{
    return abc_table->abc;
}

#endif
