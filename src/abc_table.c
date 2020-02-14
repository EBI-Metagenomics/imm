#include "imm/abc_table.h"
#include "free.h"
#include "imm/abc.h"
#include "imm/lprob.h"
#include "imm/report.h"
#include <stdlib.h>
#include <string.h>

struct imm_abc_table
{
    struct imm_abc const* abc;
    double                lprobs[];
};

struct imm_abc_table const* imm_abc_table_create(struct imm_abc const* abc, double const* lprobs)
{
    struct imm_abc_table* abc_table =
        malloc(sizeof(struct imm_abc_table) + sizeof(double) * imm_abc_length(abc));
    abc_table->abc = abc;
    memcpy(abc_table->lprobs, lprobs, sizeof(double) * imm_abc_length(abc));
    return abc_table;
}

double imm_abc_table_lprob(struct imm_abc_table const* abc_table, char const symbol)
{
    int idx = imm_abc_symbol_idx(abc_table->abc, symbol);
    if (idx < 0) {
        imm_error("symbol not found");
        return imm_lprob_invalid();
    }
    size_t i = (size_t)idx;
    return abc_table->lprobs[i];
}

void imm_abc_table_destroy(struct imm_abc_table const* abc_table) { free_c(abc_table); }

struct imm_abc const* imm_abc_table_get_abc(struct imm_abc_table const* abc_table) { return abc_table->abc; }
