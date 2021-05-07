#include "hope/hope.h"
#include "imm/imm.h"

void test_amino_lprob(void);

int main(void)
{
    test_amino_lprob();
    return hope_status();
}

void test_amino_lprob(void)
{
    struct imm_amino const *amino = &imm_amino_default;

    imm_float const lprobs[IMM_AMINO_NSYMBOLS] = {
        imm_lprob_zero(), imm_log(1), [19] = imm_log(19)};
    struct imm_amino_lprob aminop;
    imm_amino_lprob_init(&aminop, amino, lprobs);

    COND(imm_lprob_is_zero(imm_amino_lprob_get(&aminop, 'A')));
    CLOSE(imm_amino_lprob_get(&aminop, 'C'), imm_log(1));
    CLOSE(imm_amino_lprob_get(&aminop, 'Y'), imm_log(19));
}
