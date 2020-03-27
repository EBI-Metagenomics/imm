#include "seq_code.h"
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

    struct seq_code const* seq_code = seq_code_create(abc, min_seq, max_seq);

    struct imm_seq const* seq = imm_seq_create("", abc);
    cass_equal_uint64(seq_code_encode(seq_code, seq), 0);
    imm_seq_destroy(seq);

    seq = imm_seq_create("A", abc);
    cass_equal_uint64(seq_code_encode(seq_code, seq), 1);
    imm_seq_destroy(seq);

    seq = imm_seq_create("T", abc);
    cass_equal_uint64(seq_code_encode(seq_code, seq), 3);
    imm_seq_destroy(seq);

    seq = imm_seq_create("AA", abc);
    cass_equal_uint64(seq_code_encode(seq_code, seq) - seq_code_offset(seq_code, 2), 0);
    imm_seq_destroy(seq);

    seq = imm_seq_create("TA", abc);
    cass_equal_uint64(seq_code_encode(seq_code, seq) - seq_code_offset(seq_code, 2), 6);
    imm_seq_destroy(seq);

    cass_equal_uint64(seq_code_size(seq_code, min_seq), 13);
    cass_equal_uint64(seq_code_size(seq_code, 1), 12);
    cass_equal_uint64(seq_code_size(seq_code, 2), 9);

    seq_code_destroy(seq_code);

    imm_abc_destroy(abc);
}

void test_seq_code2(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    unsigned              min_seq = 2;
    unsigned              max_seq = 4;

    struct seq_code const* seq_code = seq_code_create(abc, min_seq, max_seq);

    struct imm_seq const* seq = imm_seq_create("AA", abc);
    cass_equal_uint64(seq_code_encode(seq_code, seq), 0);
    imm_seq_destroy(seq);

    seq = imm_seq_create("AC", abc);
    cass_equal_uint64(seq_code_encode(seq_code, seq), 1);
    imm_seq_destroy(seq);

    seq = imm_seq_create("TT", abc);
    cass_equal_uint64(seq_code_encode(seq_code, seq), 15);
    imm_seq_destroy(seq);

    seq = imm_seq_create("AAA", abc);
    cass_equal_uint64(seq_code_encode(seq_code, seq), 16);
    imm_seq_destroy(seq);

    seq = imm_seq_create("AAA", abc);
    cass_equal_uint64(seq_code_encode(seq_code, seq) - seq_code_offset(seq_code, 3), 0);
    imm_seq_destroy(seq);

    seq = imm_seq_create("TTT", abc);
    cass_equal_uint64(seq_code_encode(seq_code, seq), 79);
    imm_seq_destroy(seq);

    seq = imm_seq_create("TTT", abc);
    cass_equal_uint64(seq_code_encode(seq_code, seq) - seq_code_offset(seq_code, 3), 63);
    imm_seq_destroy(seq);

    seq = imm_seq_create("TTTT", abc);
    cass_equal_uint64(seq_code_encode(seq_code, seq), 335);
    imm_seq_destroy(seq);

    seq_code_destroy(seq_code);

    imm_abc_destroy(abc);
}
