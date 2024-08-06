#include "imm_codon.h"
#include <assert.h>

void imm_codon_set(struct imm_codon *codon, int id_a, int id_b, int id_c)
{
  struct imm_abc const *abc = &codon->nuclt->super;

  assert(!(imm__abc_symbol_type(abc, id_a) == IMM_SYM_NULL ||
           imm__abc_symbol_type(abc, id_b) == IMM_SYM_NULL ||
           imm__abc_symbol_type(abc, id_c) == IMM_SYM_NULL));

  codon->a = imm__abc_symbol_idx(abc, id_a);
  codon->b = imm__abc_symbol_idx(abc, id_b);
  codon->c = imm__abc_symbol_idx(abc, id_c);
}

char imm_codon_asym(struct imm_codon const *codon)
{
  return imm_abc_symbols(&codon->nuclt->super)[codon->a];
}

char imm_codon_bsym(struct imm_codon const *codon)
{
  return imm_abc_symbols(&codon->nuclt->super)[codon->b];
}

char imm_codon_csym(struct imm_codon const *codon)
{
  return imm_abc_symbols(&codon->nuclt->super)[codon->c];
}

struct imm_codon imm_codon(struct imm_nuclt const *nuclt, int id_a, int id_b,
                           int id_c)
{
  struct imm_codon codon = {.nuclt = nuclt};
  imm_codon_set(&codon, id_a, id_b, id_c);
  return codon;
}

struct imm_codon imm_codon_any(struct imm_nuclt const *nuclt)
{
  int any = imm_abc_any_symbol_id(&nuclt->super);
  return imm_codon(nuclt, any, any, any);
}

struct imm_codon imm_codon_from_symbols(struct imm_nuclt const *nuclt, char a,
                                        char b, char c)
{
  return imm_codon(nuclt, imm_sym_id(a), imm_sym_id(b), imm_sym_id(c));
}
