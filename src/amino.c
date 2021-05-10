#include "imm/amino.h"
#include "abc.h"

int imm_amino_init(struct imm_amino *amino, char const symbols[IMM_AMINO_SIZE],
                   char any_symbol)
{
    struct imm_abc_vtable vtable = {IMM_AMINO, amino};
    return abc_init(&amino->super, IMM_AMINO_SIZE, symbols, any_symbol, vtable);
}
