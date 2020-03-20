#include "imm/abc_table.h"
#include "free.h"
#include <stdlib.h>
#include <string.h>

struct imm_abc_table const* imm_abc_table_create(struct imm_abc const* abc, double const* lprobs)
{
    struct imm_abc_table* abc_table =
        malloc(sizeof(struct imm_abc_table) + sizeof(double) * imm_abc_length(abc));
    abc_table->abc = abc;
    memcpy(abc_table->lprobs, lprobs, sizeof(double) * imm_abc_length(abc));
    return abc_table;
}

void imm_abc_table_destroy(struct imm_abc_table const* abc_table) { imm_free(abc_table); }
