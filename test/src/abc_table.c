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
    imm_float const             lprobs[4] = {logf(0.2f), logf(0.01f), logf(1.0f), logf(0.5f)};
    struct imm_abc_table const* abc_table = imm_abc_table_create(abc, lprobs);

    cass_close(imm_abc_table_lprob(abc_table, 'A'), logf(0.2f));
    cass_close(imm_abc_table_lprob(abc_table, 'C'), logf(0.01f));
    cass_cond(!imm_lprob_is_valid(imm_abc_table_lprob(abc_table, 'X')));
    cass_close(imm_abc_table_lprob(abc_table, 'T'), logf(0.5f));

    imm_abc_destroy(abc);
    imm_abc_table_destroy(abc_table);
}
