#ifndef IMM_GENCODE_H
#define IMM_GENCODE_H

#include "codon.h"
#include "compiler.h"
#include "dna.h"

struct imm_gencode
{
  char const *name1;
  char const *name2;
  int const id;
  char const *ncbieaa;
  char const *sncbieaa;
  char const *base1;
  char const *base2;
  char const *base3;
};

IMM_API extern struct imm_dna const *const imm_gencode_dna;

IMM_API struct imm_gencode const *imm_gencode_get(int table_id);
IMM_API int imm_gencode_size(struct imm_gencode const *);
IMM_API struct imm_codon imm_gencode_codon(struct imm_gencode const *, int idx);
IMM_API char imm_gencode_amino(struct imm_gencode const *, int idx);
IMM_API char imm_gencode_decode(struct imm_gencode const *,
                                struct imm_codon codon);

#endif
