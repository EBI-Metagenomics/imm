#include "hope.h"
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
    struct imm_nuclt const *nuclt = imm_super(&imm_dna_iupac);
    struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);

    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "ATG"), imm_log(0.8));
    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "ATT"), imm_log(0.1));

    struct imm_codon_marg codonm = imm_codon_marg(&codonp);

    CLOSE(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "ATG")), imm_log(0.8));

    CLOSE(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "ATT")), imm_log(0.1));

    COND(imm_lprob_is_zero(
        imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "TTT"))));
}

void test_codonm_marginal(void)
{
    struct imm_nuclt const *nuclt = imm_super(&imm_dna_iupac);
    struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);

    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "ATG"), imm_log(0.8));
    imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "ATT"), imm_log(0.1));

    struct imm_codon_marg codonm = imm_codon_marg(&codonp);

    CLOSE(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "ATG")), imm_log(0.8));
    CLOSE(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "ATT")), imm_log(0.1));
    CLOSE(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "ATX")), imm_log(0.9));
    CLOSE(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "AXX")), imm_log(0.9));
    CLOSE(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "XXX")), imm_log(0.9));
    CLOSE(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "XTX")), imm_log(0.9));
    CLOSE(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "XXG")), imm_log(0.8));
    CLOSE(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "XXT")), imm_log(0.1));
}
