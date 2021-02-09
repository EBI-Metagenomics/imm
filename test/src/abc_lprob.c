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
    imm_float const             lprobs[4] = {logf(0.2f), logf(0.01f), logf(1.0f), logf(0.5f)};
    struct imm_abc_lprob const* abc_lprob = imm_abc_lprob_create(abc, lprobs);

    cass_close(imm_abc_lprob_get(abc_lprob, 'A'), logf(0.2f));
    cass_close(imm_abc_lprob_get(abc_lprob, 'C'), logf(0.01f));
    cass_cond(!imm_lprob_is_valid(imm_abc_lprob_get(abc_lprob, 'X')));
    cass_close(imm_abc_lprob_get(abc_lprob, 'T'), logf(0.5f));

    imm_abc_destroy(abc);
    imm_abc_lprob_destroy(abc_lprob);
}
