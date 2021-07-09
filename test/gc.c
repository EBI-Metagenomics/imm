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

    struct imm_nuclt const *nuclt = &imm_gc_dna()->super;
    struct imm_codon codons[2] = {imm_codon(nuclt, 'T', 'T', 'T'),
                                  imm_codon(nuclt, 'T', 'T', 'C')};

    unsigned idx = 0;
    for (unsigned i = 0; i < imm_gc_size(); ++i)
    {
        if (imm_gc_aa(1, i) == 'F')
        {
            EQ(imm_gc_codon(1, i).a, codons[idx].a);
            EQ(imm_gc_codon(1, i).b, codons[idx].b);
            EQ(imm_gc_codon(1, i).c, codons[idx].c);
            idx++;
        }
    }
}

void test_gc_l(void)
{
    EQ(imm_gc_name1(1), "Standard");
    EQ(imm_gc_name2(1), "SGC0");

    struct imm_nuclt const *nuclt = &imm_gc_dna()->super;
    struct imm_codon codons[6] = {
        imm_codon(nuclt, 'T', 'T', 'A'), imm_codon(nuclt, 'T', 'T', 'G'),
        imm_codon(nuclt, 'C', 'T', 'T'), imm_codon(nuclt, 'C', 'T', 'C'),
        imm_codon(nuclt, 'C', 'T', 'A'), imm_codon(nuclt, 'C', 'T', 'G')};

    unsigned idx = 0;
    for (unsigned i = 0; i < imm_gc_size(); ++i)
    {
        if (imm_gc_aa(1, i) == 'L')
        {
            EQ(imm_gc_codon(1, i).a, codons[idx].a);
            EQ(imm_gc_codon(1, i).b, codons[idx].b);
            EQ(imm_gc_codon(1, i).c, codons[idx].c);
            idx++;
        }
    }
}

void test_gc_p(void)
{
    EQ(imm_gc_name1(1), "Standard");
    EQ(imm_gc_name2(1), "SGC0");

    struct imm_nuclt const *nuclt = &imm_gc_dna()->super;
    struct imm_codon codons[4] = {
        imm_codon(nuclt, 'C', 'C', 'T'), imm_codon(nuclt, 'C', 'C', 'C'),
        imm_codon(nuclt, 'C', 'C', 'A'), imm_codon(nuclt, 'C', 'C', 'G')};

    unsigned idx = 0;
    for (unsigned i = 0; i < imm_gc_size(); ++i)
    {
        if (imm_gc_aa(1, i) == 'P')
        {
            EQ(imm_gc_codon(1, i).a, codons[idx].a);
            EQ(imm_gc_codon(1, i).b, codons[idx].b);
            EQ(imm_gc_codon(1, i).c, codons[idx].c);
            idx++;
        }
    }
}

void test_gc_decode(void)
{
    EQ(imm_gc_name1(1), "Standard");
    EQ(imm_gc_name2(1), "SGC0");
    EQ(imm_gc_decode(1, 'C', 'C', 'G'), 'P');
    EQ(imm_gc_decode(1, 'T', 'A', 'G'), '*');
}
