#ifndef IMM_ABC_TABLE_H
#define IMM_ABC_TABLE_H

#include "imm/abc.h"
#include "imm/export.h"
#include "imm/float.h"
#include "imm/lprob.h"

struct imm_abc_table
{
    struct imm_abc const* abc;
    imm_float             lprobs[];
};

IMM_API struct imm_abc_table const* imm_abc_table_create(struct imm_abc const* abc,
                                                         imm_float const*      lprobs);
IMM_API void                        imm_abc_table_destroy(struct imm_abc_table const* abc_table);
static inline struct imm_abc const* imm_abc_table_abc(struct imm_abc_table const* abc_table);
static inline imm_float imm_abc_table_lprob(struct imm_abc_table const* abc_table, char symbol);

static inline struct imm_abc const* imm_abc_table_abc(struct imm_abc_table const* abc_table)
{
    return abc_table->abc;
}

static inline imm_float imm_abc_table_lprob(struct imm_abc_table const* abc_table, char symbol)
{
    uint8_t idx = imm_abc_symbol_idx(abc_table->abc, symbol);
    if (idx == IMM_ABC_INVALID_IDX) {
        imm_error("symbol not found");
        return imm_lprob_invalid();
    }
    return abc_table->lprobs[idx];
}

#endif
