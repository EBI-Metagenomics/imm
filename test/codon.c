#include "hope/hope.h"
#include "imm/imm.h"

void test_codon(void);

int main(void)
{
    test_codon();
    return hope_status();
}

void test_codon(void)
{
    struct imm_dna const *dna = &imm_dna_default;
    struct imm_abc const *abc = imm_super(imm_super(dna));

    struct imm_codon codon = IMM_CODON_INIT(imm_super(dna));

    EQ(codon.a, imm_abc_any_symbol_idx(abc));
    EQ(codon.b, imm_abc_any_symbol_idx(abc));
    EQ(codon.c, imm_abc_any_symbol_idx(abc));

    EQ(imm_codon_set(&codon, IMM_TRIPLET('G', 'G', 'G')), IMM_SUCCESS);
    EQ(imm_codon_set(&codon, IMM_TRIPLET('J', 'G', 'G')), IMM_ILLEGALARG);
}
