#include "hope/hope.h"
#include "imm/imm.h"

void test_gc_f(void);
void test_gc_l(void);
void test_gc_p(void);
void test_gc_decode(void);

int main(void)
{
    test_gc_f();
    test_gc_l();
    test_gc_p();
    test_gc_decode();
    return hope_status();
}

void test_gc_f(void)
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

void test_gc_l(void)
{
    EQ(imm_gc_name1(1), "Standard");
    EQ(imm_gc_name2(1), "SGC0");
    EQ(imm_gc_ncodons(1, 'L'), 6);
    struct imm_codon codons[6] = {
        imm_gc_codon(1, 'L', 0), imm_gc_codon(1, 'L', 1),
        imm_gc_codon(1, 'L', 2), imm_gc_codon(1, 'L', 3),
        imm_gc_codon(1, 'L', 4), imm_gc_codon(1, 'L', 5)};

    EQ(imm_codon_asym(codons + 0), 'T');
    EQ(imm_codon_bsym(codons + 0), 'T');
    EQ(imm_codon_csym(codons + 0), 'A');
    EQ(imm_codon_asym(codons + 1), 'T');
    EQ(imm_codon_bsym(codons + 1), 'T');
    EQ(imm_codon_csym(codons + 1), 'G');
    EQ(imm_codon_asym(codons + 2), 'C');
    EQ(imm_codon_bsym(codons + 2), 'T');
    EQ(imm_codon_csym(codons + 2), 'T');
    EQ(imm_codon_asym(codons + 3), 'C');
    EQ(imm_codon_bsym(codons + 3), 'T');
    EQ(imm_codon_csym(codons + 3), 'C');
    EQ(imm_codon_asym(codons + 4), 'C');
    EQ(imm_codon_bsym(codons + 4), 'T');
    EQ(imm_codon_csym(codons + 4), 'A');
    EQ(imm_codon_asym(codons + 5), 'C');
    EQ(imm_codon_bsym(codons + 5), 'T');
    EQ(imm_codon_csym(codons + 5), 'G');
}

void test_gc_p(void)
{
    EQ(imm_gc_name1(1), "Standard");
    EQ(imm_gc_name2(1), "SGC0");
    EQ(imm_gc_ncodons(1, 'P'), 4);
    struct imm_codon codons[4] = {
        imm_gc_codon(1, 'P', 0), imm_gc_codon(1, 'P', 1),
        imm_gc_codon(1, 'P', 2), imm_gc_codon(1, 'P', 3)};

    EQ(imm_codon_asym(codons + 0), 'C');
    EQ(imm_codon_bsym(codons + 0), 'C');
    EQ(imm_codon_csym(codons + 0), 'T');
    EQ(imm_codon_asym(codons + 1), 'C');
    EQ(imm_codon_bsym(codons + 1), 'C');
    EQ(imm_codon_csym(codons + 1), 'C');
    EQ(imm_codon_asym(codons + 2), 'C');
    EQ(imm_codon_bsym(codons + 2), 'C');
    EQ(imm_codon_csym(codons + 2), 'A');
    EQ(imm_codon_asym(codons + 3), 'C');
    EQ(imm_codon_bsym(codons + 3), 'C');
    EQ(imm_codon_csym(codons + 3), 'G');
}

void test_gc_decode(void)
{
    EQ(imm_gc_name1(1), "Standard");
    EQ(imm_gc_name2(1), "SGC0");
    EQ(imm_gc_decode(1, 'C', 'C', 'G'), 'P');
    EQ(imm_gc_decode(1, 'T', 'A', 'G'), '*');
}
