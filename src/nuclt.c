#include "imm/nuclt.h"
#include "abc.h"

int imm_nuclt_init(struct imm_nuclt *nuclt,
                   char const symbols[IMM_NUCLT_NSYMBOLS], char any_symbol)
{
    struct imm_abc_vtable vtable = {IMM_NUCLT, nuclt};
    return abc_init(&nuclt->super, IMM_NUCLT_NSYMBOLS, symbols, any_symbol,
                    vtable);
}
