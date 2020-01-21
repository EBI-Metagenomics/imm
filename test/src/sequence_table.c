#include "cass.h"
#include "imm/imm.h"

void test_sequence_table(void);

int main(void)
{
    test_sequence_table();
    return cass_status();
}

void test_sequence_table(void)
{
    struct imm_abc* abc = imm_abc_create("ACGT", '*');

    struct imm_sequence_table* table = imm_sequence_table_create(abc);

    cass_cond(imm_lprob_is_zero(imm_sequence_table_lprob(table, "", 0)));
    cass_cond(imm_lprob_is_zero(imm_sequence_table_lprob(table, "AGT", 3)));

    imm_sequence_table_add(table, "GG", log(0.5));
    cass_close(imm_sequence_table_lprob(table, "GG", 2), log(0.5));
    cass_close(imm_sequence_table_lprob(table, "GGX", 2), log(0.5));
    cass_cond(imm_lprob_is_zero(imm_sequence_table_lprob(table, "GGT", 3)));

    imm_sequence_table_add(table, "", log(0.1));
    cass_close(imm_sequence_table_lprob(table, "", 0), log(0.1));

    cass_cond(imm_sequence_table_normalize(table) == 0);

    cass_close(imm_sequence_table_lprob(table, "GG", 2), log(0.5 / 0.6));
    cass_close(imm_sequence_table_lprob(table, "", 0), log(0.1 / 0.6));

    imm_sequence_table_destroy(table);
    imm_abc_destroy(abc);
}
