#include "eseq.h"
#include "cass.h"
#include "imm/imm.h"
#include "seq_code.h"

void test_eseq(void);

int main(void)
{
    test_eseq();
    return cass_status();
}

void test_eseq(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');

    unsigned min_seq = 2;
    unsigned max_seq = 4;

    struct seq_code const* seq_code = seq_code_create(abc, min_seq, max_seq);

    struct imm_seq const* seq = imm_seq_create("ACAGT", abc);
    struct eseq const*    eseq = seq_code_create_eseq(seq_code, seq);
    cass_cond(eseq_get(eseq, 0, 2) == 1);
    cass_cond(eseq_get(eseq, 0, 3) == 20);
    cass_cond(eseq_get(eseq, 0, 4) == 98);
    cass_cond(eseq_get(eseq, 1, 2) == 4);
    cass_cond(eseq_get(eseq, 1, 3) == 34);
    cass_cond(eseq_get(eseq, 1, 4) == 155);
    cass_cond(eseq_get(eseq, 2, 2) == 2);
    cass_cond(eseq_get(eseq, 2, 3) == 27);
    eseq_destroy(eseq);
    imm_seq_destroy(seq);

    seq_code_destroy(seq_code);
    imm_abc_destroy(abc);
}
