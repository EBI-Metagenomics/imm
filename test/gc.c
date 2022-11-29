#include "hope.h"
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
    eq(imm_gc_name1(1), "Standard");
    eq(imm_gc_name2(1), "SGC0");

    struct imm_nuclt const *nuclt = &imm_gc_dna->super;
    struct imm_codon codons[2] = {IMM_CODON(nuclt, "TTT"),
                                  IMM_CODON(nuclt, "TTC")};

    unsigned idx = 0;
    for (unsigned i = 0; i < imm_gc_size(); ++i)
    {
        if (imm_gc_aa(1, i) == 'F')
        {
            eq(imm_gc_codon(1, i).a, codons[idx].a);
            eq(imm_gc_codon(1, i).b, codons[idx].b);
            eq(imm_gc_codon(1, i).c, codons[idx].c);
            idx++;
        }
    }
}

void test_gc_l(void)
{
    eq(imm_gc_name1(1), "Standard");
    eq(imm_gc_name2(1), "SGC0");

    struct imm_nuclt const *nuclt = &imm_gc_dna->super;
    struct imm_codon codons[6] = {
        IMM_CODON(nuclt, "TTA"), IMM_CODON(nuclt, "TTG"),
        IMM_CODON(nuclt, "CTT"), IMM_CODON(nuclt, "CTC"),
        IMM_CODON(nuclt, "CTA"), IMM_CODON(nuclt, "CTG")};

    unsigned idx = 0;
    for (unsigned i = 0; i < imm_gc_size(); ++i)
    {
        if (imm_gc_aa(1, i) == 'L')
        {
            eq(imm_gc_codon(1, i).a, codons[idx].a);
            eq(imm_gc_codon(1, i).b, codons[idx].b);
            eq(imm_gc_codon(1, i).c, codons[idx].c);
            idx++;
        }
    }
}

void test_gc_p(void)
{
    eq(imm_gc_name1(1), "Standard");
    eq(imm_gc_name2(1), "SGC0");

    struct imm_nuclt const *nuclt = &imm_gc_dna->super;
    struct imm_codon codons[4] = {
        IMM_CODON(nuclt, "CCT"), IMM_CODON(nuclt, "CCC"),
        IMM_CODON(nuclt, "CCA"), IMM_CODON(nuclt, "CCG")};

    unsigned idx = 0;
    for (unsigned i = 0; i < imm_gc_size(); ++i)
    {
        if (imm_gc_aa(1, i) == 'P')
        {
            eq(imm_gc_codon(1, i).a, codons[idx].a);
            eq(imm_gc_codon(1, i).b, codons[idx].b);
            eq(imm_gc_codon(1, i).c, codons[idx].c);
            idx++;
        }
    }
}

void test_gc_decode(void)
{
    eq(imm_gc_name1(1), "Standard");
    eq(imm_gc_name2(1), "SGC0");
    struct imm_nuclt const *nuclt = &imm_gc_dna->super;
    eq(imm_gc_decode(1, IMM_CODON(nuclt, "CCG")), 'P');
    eq(imm_gc_decode(1, IMM_CODON(nuclt, "TAG")), '*');
}
