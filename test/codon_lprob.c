#include "hope/hope.h"
#include "imm/imm.h"

void test_codon_lprob(void);

int main(void)
{
    test_codon_lprob();
    return hope_status();
}

void test_codon_lprob(void)
{
    struct imm_dna const *dna = &imm_dna_default;
    struct imm_nuclt const *nuclt = imm_super(dna);

    struct imm_codon_lprob codonp = imm_codon_lprob(imm_super(dna));

    EQ(imm_codon_lprob_normalize(&codonp), IMM_ILLEGALARG);

    struct imm_codon codon = imm_codon(nuclt, 'A', 'C', 'C');

    COND(imm_lprob_is_zero(imm_codon_lprob_get(&codonp, codon)));
    imm_codon_lprob_set(&codonp, codon, imm_log(0.5));
    CLOSE(imm_codon_lprob_get(&codonp, codon), imm_log(0.5));

    EQ(imm_codon_lprob_normalize(&codonp), IMM_SUCCESS);
    CLOSE(imm_codon_lprob_get(&codonp, codon), imm_log(1.0));

    codon = imm_codon(nuclt, 'A', 'C', 'X');
    COND(imm_lprob_is_zero(imm_codon_lprob_get(&codonp, codon)));
}
