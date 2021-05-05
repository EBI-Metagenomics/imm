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

    struct imm_codon codon = IMM_CODON_INIT(imm_super(dna));

    EQ(imm_codon_get(&codon)->a, imm_abc_any_symbol(imm_super(imm_super(dna))));
    EQ(imm_codon_get(&codon)->b, imm_abc_any_symbol(imm_super(imm_super(dna))));
    EQ(imm_codon_get(&codon)->c, imm_abc_any_symbol(imm_super(imm_super(dna))));

    EQ(imm_codon_set(&codon, IMM_TRIPLET_INIT('G', 'G', 'G')), IMM_SUCCESS);
    EQ(imm_codon_set(&codon, IMM_TRIPLET_INIT('J', 'G', 'G')), IMM_ILLEGALARG);
}
