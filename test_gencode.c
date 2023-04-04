#include "array_size.h"
#include "gencode.h"
#include "test_helper.h"

static void gc_f(void)
{
  struct imm_gencode const *gc = imm_gencode_get(1);
  lsequal(gc->name1, "Standard");
  lsequal(gc->name2, "SGC0");

  struct imm_nuclt const *nuclt = &imm_gc_dna->super;
  struct imm_codon codons[2] = {IMM_CODON(nuclt, "TTT"),
                                IMM_CODON(nuclt, "TTC")};

  unsigned idx = 0;
  for (unsigned i = 0; i < imm_gencode_size(gc); ++i)
  {
    if (imm_gencode_aa(gc, i) == 'F')
    {
      eq(imm_gencode_codon(gc, i).a, codons[idx].a);
      eq(imm_gencode_codon(gc, i).b, codons[idx].b);
      eq(imm_gencode_codon(gc, i).c, codons[idx].c);
      idx++;
    }
  }
}

static void gc_l(void)
{
  struct imm_gencode const *gc = imm_gencode_get(1);
  lsequal(gc->name1, "Standard");
  lsequal(gc->name2, "SGC0");

  struct imm_nuclt const *nuclt = &imm_gc_dna->super;
  struct imm_codon codons[6] = {
      IMM_CODON(nuclt, "TTA"), IMM_CODON(nuclt, "TTG"),
      IMM_CODON(nuclt, "CTT"), IMM_CODON(nuclt, "CTC"),
      IMM_CODON(nuclt, "CTA"), IMM_CODON(nuclt, "CTG")};

  unsigned idx = 0;
  for (unsigned i = 0; i < imm_gencode_size(gc); ++i)
  {
    if (imm_gencode_aa(gc, i) == 'L')
    {
      eq(imm_gencode_codon(gc, i).a, codons[idx].a);
      eq(imm_gencode_codon(gc, i).b, codons[idx].b);
      eq(imm_gencode_codon(gc, i).c, codons[idx].c);
      idx++;
    }
  }
}

static void gc_p(void)
{
  struct imm_gencode const *gc = imm_gencode_get(1);
  lsequal(gc->name1, "Standard");
  lsequal(gc->name2, "SGC0");

  struct imm_nuclt const *nuclt = &imm_gc_dna->super;
  struct imm_codon codons[4] = {
      IMM_CODON(nuclt, "CCT"), IMM_CODON(nuclt, "CCC"), IMM_CODON(nuclt, "CCA"),
      IMM_CODON(nuclt, "CCG")};

  unsigned idx = 0;
  for (unsigned i = 0; i < imm_gencode_size(gc); ++i)
  {
    if (imm_gencode_aa(gc, i) == 'P')
    {
      eq(imm_gencode_codon(gc, i).a, codons[idx].a);
      eq(imm_gencode_codon(gc, i).b, codons[idx].b);
      eq(imm_gencode_codon(gc, i).c, codons[idx].c);
      idx++;
    }
  }
}

static void gc_decode(void)
{
  struct imm_gencode const *gc = imm_gencode_get(1);
  lsequal(gc->name1, "Standard");
  lsequal(gc->name2, "SGC0");
  struct imm_nuclt const *nuclt = &imm_gc_dna->super;
  eq(imm_gencode_decode(gc, IMM_CODON(nuclt, "CCG")), 'P');
  eq(imm_gencode_decode(gc, IMM_CODON(nuclt, "TAG")), '*');
}

int main(void)
{
  lrun("gc_f", gc_f);
  lrun("gc_l", gc_l);
  lrun("gc_p", gc_p);
  lrun("gc_decode", gc_decode);
  return lfails != 0;
}
