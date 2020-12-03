#include "cass.h"
#include "imm/imm.h"

void test_seq_table(void);

int main(void)
{
    test_seq_table();
    return cass_status();
}

void test_seq_table(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');

    struct imm_seq_table* table = imm_seq_table_create(abc);

    struct imm_seq const* seq = imm_seq_create("", abc);
    cass_cond(imm_lprob_is_zero(imm_seq_table_lprob(table, seq)));
    imm_seq_destroy(seq);

    seq = imm_seq_create("AGT", abc);
    cass_cond(imm_lprob_is_zero(imm_seq_table_lprob(table, seq)));
    imm_seq_destroy(seq);

    seq = imm_seq_create("GG", abc);
    cass_cond(imm_seq_table_add(table, seq, log(0.5)) == 0);
    cass_close(imm_seq_table_lprob(table, seq), log(0.5));
    imm_seq_destroy(seq);

    seq = imm_seq_create("GGT", abc);
    cass_cond(imm_lprob_is_zero(imm_seq_table_lprob(table, seq)));
    imm_seq_destroy(seq);

    seq = imm_seq_create("", abc);
    cass_cond(imm_seq_table_add(table, seq, log(0.1)) == 0);
    cass_close(imm_seq_table_lprob(table, seq), log(0.1));
    imm_seq_destroy(seq);

    cass_cond(imm_seq_table_normalize(table) == 0);

    seq = imm_seq_create("GG", abc);
    cass_close(imm_seq_table_lprob(table, seq), log(0.5 / 0.6));
    imm_seq_destroy(seq);
    seq = imm_seq_create("", abc);
    cass_close(imm_seq_table_lprob(table, seq), log(0.1 / 0.6));
    imm_seq_destroy(seq);

    imm_seq_table_destroy(table);
    imm_abc_destroy(abc);
}
