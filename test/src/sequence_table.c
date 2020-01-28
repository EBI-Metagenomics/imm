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
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');

    struct imm_seq_table* table = imm_seq_table_create(abc);

    cass_cond(imm_lprob_is_zero(imm_seq_table_lprob(table, IMM_SEQ(""))));
    cass_cond(imm_lprob_is_zero(imm_seq_table_lprob(table, IMM_SEQ("AGT"))));

    cass_cond(imm_seq_table_add(table, IMM_SEQ("GG"), log(0.5)) == 0);
    cass_close(imm_seq_table_lprob(table, IMM_SEQ("GG")), log(0.5));
    cass_cond(imm_lprob_is_zero(imm_seq_table_lprob(table, IMM_SEQ("GGT"))));

    cass_cond(imm_seq_table_add(table, IMM_SEQ(""), log(0.1)) == 0);
    cass_close(imm_seq_table_lprob(table, IMM_SEQ("")), log(0.1));

    cass_cond(imm_seq_table_normalize(table) == 0);

    cass_close(imm_seq_table_lprob(table, IMM_SEQ("GG")), log(0.5 / 0.6));
    cass_close(imm_seq_table_lprob(table, IMM_SEQ("")), log(0.1 / 0.6));

    imm_seq_table_destroy(table);
    imm_abc_destroy(abc);
}
