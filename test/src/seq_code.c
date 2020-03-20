#include "cass.h"
#include "imm/imm.h"

void test_seq_code(void);

int main(void)
{
    test_seq_code();
    return cass_status();
}

void test_seq_code(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    unsigned              min_seq = 2;
    unsigned              max_seq = 4;

    struct seq_code const* seq_code = imm_seq_code_create(abc, min_seq, max_seq);
    printf("\n");

    struct imm_seq const* seq = imm_seq_create("AA", abc);
    cass_equal_uint64(imm_seq_code_encode(seq_code, seq), 0)
    imm_seq_destroy(seq);

    seq = imm_seq_create("AC", abc);
    cass_equal_uint64(imm_seq_code_encode(seq_code, seq), 4)
    imm_seq_destroy(seq);

    seq = imm_seq_create("TT", abc);
    cass_equal_uint64(imm_seq_code_encode(seq_code, seq), 15)
    imm_seq_destroy(seq);

    seq = imm_seq_create("AAA", abc);
    cass_equal_uint64(imm_seq_code_encode(seq_code, seq), 16)
    imm_seq_destroy(seq);

    seq = imm_seq_create("TTT", abc);
    cass_equal_uint64(imm_seq_code_encode(seq_code, seq), 79)
    imm_seq_destroy(seq);

    seq = imm_seq_create("TTTT", abc);
    cass_equal_uint64(imm_seq_code_encode(seq_code, seq), 335)
    imm_seq_destroy(seq);

    imm_seq_code_destroy(seq_code);

    imm_abc_destroy(abc);
}
