#ifndef IMM_ABC_TABLE_H
#define IMM_ABC_TABLE_H

#include "imm/abc.h"
#include "imm/export.h"
#include "imm/lprob.h"

struct imm_abc_table
{
    struct imm_abc const* abc;
    double                lprobs[];
};

IMM_API struct imm_abc_table const* imm_abc_table_create(struct imm_abc const* abc,
                                                         double const*         lprobs);
IMM_API void                        imm_abc_table_destroy(struct imm_abc_table const* abc_table);
static inline struct imm_abc const* imm_abc_table_abc(struct imm_abc_table const* abc_table);
static inline double imm_abc_table_lprob(struct imm_abc_table const* abc_table, char symbol);

static inline struct imm_abc const* imm_abc_table_abc(struct imm_abc_table const* abc_table)
{
    return abc_table->abc;
}

static inline double imm_abc_table_lprob(struct imm_abc_table const* abc_table, char symbol)
{
    unsigned idx = imm_abc_symbol_idx(abc_table->abc, symbol);
    if (idx == IMM_ABC_INVALID_IDX) {
        imm_error("symbol not found");
        return imm_lprob_invalid();
    }
    return abc_table->lprobs[idx];
}

#endif
