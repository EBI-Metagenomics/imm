#include "hope/hope.h"
#include "imm/imm.h"

void test_gc(void);
void test_abc_duplicated_alphabet(void);
void test_abc_duplicated_any_symbol(void);
void test_abc_symbol_outside_range(void);
void test_abc_any_symbol_outside_range(void);

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
    /* EQ(codons[0].b, imm_abc_symbol_idx(&codons[0].nuclt->super, 'T')); */
    /* EQ(codons[0].c, imm_abc_symbol_idx(&codons[0].nuclt->super, 'T')); */
    /* EQ(codons[1].a, imm_abc_symbol_idx(&codons[0].nuclt->super, 'T')); */
    /* EQ(codons[1].b, imm_abc_symbol_idx(&codons[0].nuclt->super, 'T')); */
    /* EQ(codons[1].c, imm_abc_symbol_idx(&codons[0].nuclt->super, 'C')); */
}
