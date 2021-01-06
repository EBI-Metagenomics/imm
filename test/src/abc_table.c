#include "cass/cass.h"
#include "imm/imm.h"

void test_abc_table(void);

int main(void)
{
    test_abc_table();
    return cass_status();
}

void test_abc_table(void)
{
    struct imm_abc const*       abc = imm_abc_create("ACGT", 'X');
    double const                lprobs[4] = {log(0.2), log(0.01), log(1.0), log(0.5)};
    struct imm_abc_table const* abc_table = imm_abc_table_create(abc, lprobs);

    cass_close(imm_abc_table_lprob(abc_table, 'A'), log(0.2));
    cass_close(imm_abc_table_lprob(abc_table, 'C'), log(0.01));
    cass_cond(!imm_lprob_is_valid(imm_abc_table_lprob(abc_table, 'X')));
    cass_close(imm_abc_table_lprob(abc_table, 'T'), log(0.5));

    imm_abc_destroy(abc);
    imm_abc_table_destroy(abc_table);
}
