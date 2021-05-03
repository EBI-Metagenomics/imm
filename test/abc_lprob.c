#include "hope/hope.h"
#include "imm/imm.h"

void test_abc_lprob(void);

int main(void)
{
    test_abc_lprob();
    return hope_status();
}

void test_abc_lprob(void)
{
    struct imm_abc const *abc = imm_abc_new(4, "ACGT", 'X');
    imm_float const lprobs[4] = {imm_log(0.2), imm_log(0.01), imm_log(1.0),
                                 imm_log(0.5)};
    struct imm_abc_lprob const *abc_lprob = imm_abc_lprob_new(abc, lprobs);

    CLOSE(imm_abc_lprob_get(abc_lprob, 'A'), imm_log(0.2));
    CLOSE(imm_abc_lprob_get(abc_lprob, 'C'), imm_log(0.01));
    COND(!imm_lprob_is_valid(imm_abc_lprob_get(abc_lprob, 'X')));
    CLOSE(imm_abc_lprob_get(abc_lprob, 'T'), imm_log(0.5));

    imm_del(abc);
    imm_del(abc_lprob);
}
