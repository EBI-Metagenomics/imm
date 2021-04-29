#include "cass/cass.h"
#include "imm/imm.h"

void test_amino_success1(void);
void test_amino_success2(void);
void test_amino_lprob(void);

int main(void)
{
    test_amino_success1();
    test_amino_success2();
    test_amino_lprob();
    return cass_status();
}

void test_amino_success1(void)
{
    struct imm_amino const *amino = imm_amino_new();
    cass_cond(amino != NULL);
    imm_del(amino);
}

void test_amino_success2(void)
{
    struct imm_amino const *amino = imm_amino_new();
    cass_cond(amino != NULL);
    struct imm_abc const *abc = imm_super(amino);
    cass_cond(abc != NULL);
    imm_del(abc);
}

void test_amino_lprob(void)
{
    struct imm_amino const *amino = imm_amino_new();
    imm_float const zero = imm_lprob_zero();
    imm_float const lprobs[IMM_AMINO_NSYMBOLS] = {
        zero, imm_log(1), [19] = imm_log(19)};

    struct imm_abc_lprob const *lprob =
        imm_abc_lprob_new(imm_super(amino), lprobs);

    cass_cond(imm_lprob_is_zero(imm_abc_lprob_get(lprob, 'A')));
    cass_close(imm_abc_lprob_get(lprob, 'C'), log(1));
    cass_close(imm_abc_lprob_get(lprob, 'Y'), log(19));

    imm_del(amino);
    imm_del(lprob);
}
