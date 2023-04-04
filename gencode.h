#ifndef IMM_GENCODE_H
#define IMM_GENCODE_H

#include "export.h"

struct imm_gencode
{
  char const *name1;
  char const *name2;
  unsigned const id;
  char const *ncbieaa;
  char const *sncbieaa;
  char const *base1;
  char const *base2;
  char const *base3;
};

struct imm_gencode const *imm_gencode_get(unsigned table_id);

#endif
