#include "imm/dna.h"
#include "abc.h"

int imm_dna_init(struct imm_dna *dna, char const symbols[IMM_DNA_NSYMBOLS],
                 char any_symbol)
{
    struct imm_abc_vtable vtable = {IMM_DNA, dna};
    return abc_init(&dna->super.super, IMM_DNA_NSYMBOLS, symbols, any_symbol,
                    vtable);
}
