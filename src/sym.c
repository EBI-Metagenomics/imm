#include "imm/sym.h"
#include "common/common.h"

void imm_sym_init(struct imm_sym *sym)
{
    for (unsigned i = 0; i < ARRAY_SIZE(sym->idx); ++i)
        sym->idx[i] = IMM_SYM_NULL_IDX;
}
