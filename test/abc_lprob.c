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
    struct imm_abc abc = imm_abc_empty;
    imm_abc_init(&abc, IMM_STR("ACGT"), 'X');
    imm_float const lprobs[4] = {imm_log(0.2), imm_log(0.01), imm_log(1.0),
                                 imm_log(0.5)};
    struct imm_abc_lprob lprob = imm_abc_lprob_init(&abc, lprobs);

    CLOSE(imm_abc_lprob_get(&lprob, 'A'), imm_log(0.2));
    CLOSE(imm_abc_lprob_get(&lprob, 'C'), imm_log(0.01));
    CLOSE(imm_abc_lprob_get(&lprob, 'T'), imm_log(0.5));
}
