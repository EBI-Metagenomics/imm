#include "amino.h"
#include "abc.h"
#include "common/common.h"
#include "imm/amino.h"

static void del(struct imm_abc const *abc)
{
    struct imm_amino const *amino = abc->vtable.derived;
    free((void *)amino);
    abc_del(abc);
}

struct imm_amino const *imm_amino_new(void)
{
    static char const symbols[IMM_AMINO_NSYMBOLS + 1] = IMM_AMINO_SYMBOLS;
    struct imm_amino *amino = xmalloc(sizeof(*amino));
    struct imm_abc_vtable vtable = {del, IMM_AMINO, amino};
    amino->super =
        abc_new(IMM_AMINO_NSYMBOLS, symbols, IMM_AMINO_ANY_SYMBOL, vtable);
    return amino;
}

void imm_amino_del(struct imm_amino const *amino)
{
    amino->super->vtable.del(amino->super);
}
