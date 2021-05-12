#include "imm/sym.h"
#include "imm/support.h"
#include "support.h"

void imm_sym_init(struct imm_sym *sym)
{
    for (unsigned i = 0; i < IMM_ARRAY_SIZE(sym->idx); ++i)
        sym->idx[i] = IMM_SYM_NULL_IDX;
}
