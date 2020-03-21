#include "cass.h"
#include "imm/imm.h"

void test_seq_code1(void);
void test_seq_code2(void);

int main(void)
{
    test_seq_code1();
    test_seq_code2();
    return cass_status();
}

void test_seq_code1(void)
{
    struct imm_abc const* abc = imm_abc_create("ACT", '*');
    unsigned              min_seq = 0;
    unsigned              max_seq = 2;

    struct seq_code const* seq_code = imm_seq_code_create(abc, min_seq, max_seq);

    struct imm_seq const* seq = imm_seq_create("", abc);
    cass_equal_uint64(imm_seq_code_encode(seq_code, min_seq, seq), 0);
    imm_seq_destroy(seq);

    seq = imm_seq_create("A", abc);
    cass_equal_uint64(imm_seq_code_encode(seq_code, min_seq, seq), 1);
    imm_seq_destroy(seq);

    seq = imm_seq_create("T", abc);
    cass_equal_uint64(imm_seq_code_encode(seq_code, min_seq, seq), 3);
    imm_seq_destroy(seq);

    seq = imm_seq_create("AA", abc);
    cass_equal_uint64(imm_seq_code_encode(seq_code, 2, seq), 0);
    imm_seq_destroy(seq);

    seq = imm_seq_create("TA", abc);
    cass_equal_uint64(imm_seq_code_encode(seq_code, 2, seq), 2);
    imm_seq_destroy(seq);

    imm_seq_code_destroy(seq_code);

    imm_abc_destroy(abc);
}

void test_seq_code2(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    unsigned              min_seq = 2;
    unsigned              max_seq = 4;

    struct seq_code const* seq_code = imm_seq_code_create(abc, min_seq, max_seq);

    struct imm_seq const* seq = imm_seq_create("AA", abc);
    cass_equal_uint64(imm_seq_code_encode(seq_code, min_seq, seq), 0);
    imm_seq_destroy(seq);

    seq = imm_seq_create("AC", abc);
    cass_equal_uint64(imm_seq_code_encode(seq_code, min_seq, seq), 4);
    imm_seq_destroy(seq);

    seq = imm_seq_create("TT", abc);
    cass_equal_uint64(imm_seq_code_encode(seq_code, min_seq, seq), 15);
    imm_seq_destroy(seq);

    seq = imm_seq_create("AAA", abc);
    cass_equal_uint64(imm_seq_code_encode(seq_code, min_seq, seq), 16);
    imm_seq_destroy(seq);

    seq = imm_seq_create("AAA", abc);
    cass_equal_uint64(imm_seq_code_encode(seq_code, 3, seq), 0);
    imm_seq_destroy(seq);

    seq = imm_seq_create("TTT", abc);
    cass_equal_uint64(imm_seq_code_encode(seq_code, min_seq, seq), 79);
    imm_seq_destroy(seq);

    seq = imm_seq_create("TTT", abc);
    cass_equal_uint64(imm_seq_code_encode(seq_code, 3, seq), 63);
    imm_seq_destroy(seq);

    seq = imm_seq_create("TTTT", abc);
    cass_equal_uint64(imm_seq_code_encode(seq_code, min_seq, seq), 335);
    imm_seq_destroy(seq);

    imm_seq_code_destroy(seq_code);

    imm_abc_destroy(abc);
}
