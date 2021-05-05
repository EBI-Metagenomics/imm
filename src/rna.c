#include "imm/rna.h"
#include "abc.h"

int imm_rna_init(struct imm_rna *rna, char const symbols[IMM_RNA_NSYMBOLS],
                 char any_symbol)
{
    struct imm_abc_vtable vtable = {IMM_RNA, rna};
    return abc_init(&rna->super.super, IMM_RNA_NSYMBOLS, symbols, any_symbol,
                    vtable);
}
