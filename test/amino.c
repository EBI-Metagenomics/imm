#include "hope/hope.h"
#include "imm/imm.h"

void test_amino_success(void);
void test_amino_lprob(void);

int main(void)
{
    test_amino_success();
    test_amino_lprob();
    return hope_status();
}

void test_amino_success(void)
{
    char const symbols[IMM_AMINO_NSYMBOLS + 1] = IMM_AMINO_SYMBOLS;

    struct imm_amino const *amino = &imm_amino_default;
    for (unsigned i = 0; i < imm_abc_len(&amino->super); ++i)
    {
        EQ(imm_abc_symbols(&amino->super)[i], symbols[i]);
        EQ(imm_abc_symbol_idx(&amino->super, symbols[i]), i);
    }

    EQ(imm_abc_any_symbol(imm_super(amino)), IMM_AMINO_ANY_SYMBOL);
    COND(imm_abc_has_symbol(imm_super(amino), 'A'));
    COND(!imm_abc_has_symbol(imm_super(amino), 'B'));

    struct imm_abc const *abc = imm_super(amino);
    NOTNULL(abc);
    EQ(imm_abc_typeid(imm_super(amino)), IMM_AMINO);
}

void test_amino_lprob(void)
{
    struct imm_amino const *amino = &imm_amino_default;
    imm_float const zero = imm_lprob_zero();
    imm_float const lprobs[IMM_AMINO_NSYMBOLS] = {
        zero, imm_log(1), [19] = imm_log(19)};

    struct imm_abc_lprob lprob;
    imm_abc_lprob_init(&lprob, imm_super(amino), lprobs);

    COND(imm_lprob_is_zero(imm_abc_lprob_get(&lprob, 'A')));
    CLOSE(imm_abc_lprob_get(&lprob, 'C'), log(1));
    CLOSE(imm_abc_lprob_get(&lprob, 'Y'), log(19));
}
