#include "cass/cass.h"
#include "imm/imm.h"

void test_abc_lprob(void);

int main(void)
{
    test_abc_lprob();
    return cass_status();
}

void test_abc_lprob(void)
{
    struct imm_abc const*       abc = imm_abc_create("ACGT", 'X');
    imm_float const             lprobs[4] = {imm_log(0.2), imm_log(0.01), imm_log(1.0), imm_log(0.5)};
    struct imm_abc_lprob const* abc_lprob = imm_abc_lprob_create(abc, lprobs);

    cass_close(imm_abc_lprob_get(abc_lprob, 'A'), imm_log(0.2));
    cass_close(imm_abc_lprob_get(abc_lprob, 'C'), imm_log(0.01));
    cass_cond(!imm_lprob_is_valid(imm_abc_lprob_get(abc_lprob, 'X')));
    cass_close(imm_abc_lprob_get(abc_lprob, 'T'), imm_log(0.5));

    imm_abc_destroy(abc);
    imm_abc_lprob_destroy(abc_lprob);
}
