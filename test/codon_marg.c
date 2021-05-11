#include "hope/hope.h"
#include "imm/imm.h"

void test_codonm_nonmarginal(void);
void test_codonm_marginal(void);

int main(void)
{
    test_codonm_nonmarginal();
    test_codonm_marginal();
    return hope_status();
}

void test_codonm_nonmarginal(void)
{
    struct imm_nuclt const *nuclt = imm_super(&imm_dna_default);
    struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);

    imm_codon_lprob_set(&codonp, imm_codon(nuclt, 'A', 'T', 'G'), imm_log(0.8));
    imm_codon_lprob_set(&codonp, imm_codon(nuclt, 'A', 'T', 'T'), imm_log(0.1));

    struct imm_codon_marg codonm = imm_codon_marg(&codonp);

    CLOSE(imm_codon_marg_lprob(&codonm, imm_codon(nuclt, 'A', 'T', 'G')),
          imm_log(0.8));

    CLOSE(imm_codon_marg_lprob(&codonm, imm_codon(nuclt, 'A', 'T', 'T')),
          imm_log(0.1));

    COND(imm_lprob_is_zero(
        imm_codon_marg_lprob(&codonm, imm_codon(nuclt, 'T', 'T', 'T'))));
}

void test_codonm_marginal(void)
{
    struct imm_nuclt const *nuclt = imm_super(&imm_dna_default);
    struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);

    imm_codon_lprob_set(&codonp, imm_codon(nuclt, 'A', 'T', 'G'), imm_log(0.8));
    imm_codon_lprob_set(&codonp, imm_codon(nuclt, 'A', 'T', 'T'), imm_log(0.1));

    struct imm_codon_marg codonm = imm_codon_marg(&codonp);

    CLOSE(imm_codon_marg_lprob(&codonm, imm_codon(nuclt, 'A', 'T', 'G')),
          imm_log(0.8));
    CLOSE(imm_codon_marg_lprob(&codonm, imm_codon(nuclt, 'A', 'T', 'T')),
          imm_log(0.1));
    CLOSE(imm_codon_marg_lprob(&codonm, imm_codon(nuclt, 'A', 'T', 'X')),
          imm_log(0.9));
    CLOSE(imm_codon_marg_lprob(&codonm, imm_codon(nuclt, 'A', 'X', 'X')),
          imm_log(0.9));
    CLOSE(imm_codon_marg_lprob(&codonm, imm_codon(nuclt, 'X', 'X', 'X')),
          imm_log(0.9));
    CLOSE(imm_codon_marg_lprob(&codonm, imm_codon(nuclt, 'X', 'T', 'X')),
          imm_log(0.9));
    CLOSE(imm_codon_marg_lprob(&codonm, imm_codon(nuclt, 'X', 'X', 'G')),
          imm_log(0.8));
    CLOSE(imm_codon_marg_lprob(&codonm, imm_codon(nuclt, 'X', 'X', 'T')),
          imm_log(0.1));
}
