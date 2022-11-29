#include "hope.h"
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
    char const symbols[IMM_AMINO_SIZE + 1] = IMM_AMINO_SYMBOLS;

    struct imm_amino const *amino = &imm_amino_iupac;
    for (unsigned i = 0; i < imm_abc_size(&amino->super); ++i)
    {
        eq(imm_abc_symbols(&amino->super)[i], symbols[i]);
        eq(imm_abc_symbol_idx(&amino->super, symbols[i]), i);
    }

    eq(imm_abc_any_symbol(imm_super(amino)), IMM_AMINO_ANY_SYMBOL);
    cond(imm_abc_has_symbol(imm_super(amino), 'A'));
    cond(!imm_abc_has_symbol(imm_super(amino), 'B'));

    notnull(imm_super(amino));
    eq(imm_typeid(amino), IMM_AMINO);
}

void test_amino_lprob(void)
{
    struct imm_amino_lprob lprob = imm_amino_lprob(
        &imm_amino_iupac,
        IMM_ARR(imm_lprob_zero(), imm_log(1), [19] = imm_log(19)));

    cond(imm_lprob_is_zero(imm_amino_lprob_get(&lprob, 'A')));
    close(imm_amino_lprob_get(&lprob, 'C'), log(1));
    close(imm_amino_lprob_get(&lprob, 'Y'), log(19));
}
