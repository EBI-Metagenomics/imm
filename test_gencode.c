#include "aye.h"
#include "imm_gencode.h"
#include <string.h>

static void gc_f(void)
{
  struct imm_gencode const *gc = imm_gencode_get(1);
  aye(!strcmp(gc->name1, "Standard"));
  aye(!strcmp(gc->name2, "SGC0"));

  struct imm_nuclt const *nuclt = &imm_gencode_dna->super;
  struct imm_codon codons[2] = {IMM_CODON(nuclt, "TTT"),
                                IMM_CODON(nuclt, "TTC")};

  int idx = 0;
  for (int i = 0; i < imm_gencode_size(gc); ++i)
  {
    if (imm_gencode_amino(gc, i) == 'F')
    {
      aye(imm_gencode_codon(gc, i).a == codons[idx].a);
      aye(imm_gencode_codon(gc, i).b == codons[idx].b);
      aye(imm_gencode_codon(gc, i).c == codons[idx].c);
      idx++;
    }
  }
}

static void gc_l(void)
{
  struct imm_gencode const *gc = imm_gencode_get(1);
  aye(!strcmp(gc->name1, "Standard"));
  aye(!strcmp(gc->name2, "SGC0"));

  struct imm_nuclt const *nuclt = &imm_gencode_dna->super;
  struct imm_codon codons[6] = {
      IMM_CODON(nuclt, "TTA"), IMM_CODON(nuclt, "TTG"),
      IMM_CODON(nuclt, "CTT"), IMM_CODON(nuclt, "CTC"),
      IMM_CODON(nuclt, "CTA"), IMM_CODON(nuclt, "CTG")};

  int idx = 0;
  for (int i = 0; i < imm_gencode_size(gc); ++i)
  {
    if (imm_gencode_amino(gc, i) == 'L')
    {
      aye(imm_gencode_codon(gc, i).a == codons[idx].a);
      aye(imm_gencode_codon(gc, i).b == codons[idx].b);
      aye(imm_gencode_codon(gc, i).c == codons[idx].c);
      idx++;
    }
  }
}

static void gc_p(void)
{
  struct imm_gencode const *gc = imm_gencode_get(1);
  aye(!strcmp(gc->name1, "Standard"));
  aye(!strcmp(gc->name2, "SGC0"));

  struct imm_nuclt const *nuclt = &imm_gencode_dna->super;
  struct imm_codon codons[4] = {
      IMM_CODON(nuclt, "CCT"), IMM_CODON(nuclt, "CCC"), IMM_CODON(nuclt, "CCA"),
      IMM_CODON(nuclt, "CCG")};

  int idx = 0;
  for (int i = 0; i < imm_gencode_size(gc); ++i)
  {
    if (imm_gencode_amino(gc, i) == 'P')
    {
      aye(imm_gencode_codon(gc, i).a == codons[idx].a);
      aye(imm_gencode_codon(gc, i).b == codons[idx].b);
      aye(imm_gencode_codon(gc, i).c == codons[idx].c);
      idx++;
    }
  }
}

static void gc_decode(void)
{
  struct imm_gencode const *gc = imm_gencode_get(1);
  aye(!strcmp(gc->name1, "Standard"));
  aye(!strcmp(gc->name2, "SGC0"));
  struct imm_nuclt const *nuclt = &imm_gencode_dna->super;
  aye(imm_gencode_decode(gc, IMM_CODON(nuclt, "CCG")) == 'P');
  aye(imm_gencode_decode(gc, IMM_CODON(nuclt, "TAG")) == '*');
}

int main(void)
{
  aye_begin();
  gc_f();
  gc_l();
  gc_p();
  gc_decode();
  return aye_end();
}
