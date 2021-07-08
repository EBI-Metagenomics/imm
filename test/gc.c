#include "hope/hope.h"
#include "imm/imm.h"

void test_gc(void);

int main(void)
{
    test_gc();
    return hope_status();
}

void test_gc(void)
{
    EQ(imm_gc_name1(1), "Standard");
    EQ(imm_gc_name2(1), "SGC0");
    EQ(imm_gc_ncodons(1, 'F'), 2);
    struct imm_codon codons[2] = {imm_gc_codon(1, 'F', 0),
                                  imm_gc_codon(1, 'F', 1)};
    EQ(imm_codon_asym(codons + 0), 'T');
    EQ(imm_codon_bsym(codons + 0), 'T');
    EQ(imm_codon_csym(codons + 0), 'T');
    EQ(imm_codon_asym(codons + 1), 'T');
    EQ(imm_codon_bsym(codons + 1), 'T');
    EQ(imm_codon_csym(codons + 1), 'C');
}
