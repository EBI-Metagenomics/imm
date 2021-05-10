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
    struct imm_dna const *dna = &imm_dna_default;
    struct imm_codon_lprob codonp = imm_codon_lprob_init(imm_super(dna));

    struct imm_codon codon = imm_codon_init(imm_super(dna));

    EQ(imm_codon_set(&codon, IMM_TRIPLET('A', 'T', 'G')), IMM_SUCCESS);
    imm_codon_lprob_set(&codonp, &codon, imm_log(0.8));

    EQ(imm_codon_set(&codon, IMM_TRIPLET('A', 'T', 'T')), IMM_SUCCESS);
    imm_codon_lprob_set(&codonp, &codon, imm_log(0.1));

    struct imm_codon_marg codonm;
    imm_codon_marg_init(&codonm, &codonp);

    EQ(imm_codon_set(&codon, IMM_TRIPLET('A', 'T', 'G')), IMM_SUCCESS);
    CLOSE(imm_codon_marg_lprob(&codonm, &codon), imm_log(0.8));
    EQ(imm_codon_set(&codon, IMM_TRIPLET('A', 'T', 'T')), IMM_SUCCESS);
    CLOSE(imm_codon_marg_lprob(&codonm, &codon), imm_log(0.1));
    EQ(imm_codon_set(&codon, IMM_TRIPLET('T', 'T', 'T')), IMM_SUCCESS);
    COND(imm_lprob_is_zero(imm_codon_marg_lprob(&codonm, &codon)));

    imm_codon_marg_deinit(&codonm);
}

void test_codonm_marginal(void)
{
    struct imm_dna const *dna = &imm_dna_default;
    struct imm_codon_lprob codonp = imm_codon_lprob_init(imm_super(dna));

    struct imm_codon codon = imm_codon_init(imm_super(dna));

    EQ(imm_codon_set(&codon, IMM_TRIPLET('A', 'T', 'G')), IMM_SUCCESS);
    imm_codon_lprob_set(&codonp, &codon, imm_log(0.8));
    EQ(imm_codon_set(&codon, IMM_TRIPLET('A', 'T', 'T')), IMM_SUCCESS);
    imm_codon_lprob_set(&codonp, &codon, imm_log(0.1));

    struct imm_codon_marg codonm;
    imm_codon_marg_init(&codonm, &codonp);

    EQ(imm_codon_set(&codon, IMM_TRIPLET('A', 'T', 'G')), IMM_SUCCESS);
    CLOSE(imm_codon_marg_lprob(&codonm, &codon), imm_log(0.8));
    EQ(imm_codon_set(&codon, IMM_TRIPLET('A', 'T', 'T')), IMM_SUCCESS);
    CLOSE(imm_codon_marg_lprob(&codonm, &codon), imm_log(0.1));
    EQ(imm_codon_set(&codon, IMM_TRIPLET('A', 'T', 'X')), IMM_SUCCESS);
    CLOSE(imm_codon_marg_lprob(&codonm, &codon), imm_log(0.9));
    EQ(imm_codon_set(&codon, IMM_TRIPLET('A', 'X', 'X')), IMM_SUCCESS);
    CLOSE(imm_codon_marg_lprob(&codonm, &codon), imm_log(0.9));
    EQ(imm_codon_set(&codon, IMM_TRIPLET('X', 'X', 'X')), IMM_SUCCESS);
    CLOSE(imm_codon_marg_lprob(&codonm, &codon), imm_log(0.9));
    EQ(imm_codon_set(&codon, IMM_TRIPLET('X', 'T', 'X')), IMM_SUCCESS);
    CLOSE(imm_codon_marg_lprob(&codonm, &codon), imm_log(0.9));
    EQ(imm_codon_set(&codon, IMM_TRIPLET('X', 'X', 'G')), IMM_SUCCESS);
    CLOSE(imm_codon_marg_lprob(&codonm, &codon), imm_log(0.8));
    EQ(imm_codon_set(&codon, IMM_TRIPLET('X', 'X', 'T')), IMM_SUCCESS);
    CLOSE(imm_codon_marg_lprob(&codonm, &codon), imm_log(0.1));

    imm_codon_marg_deinit(&codonm);
}
