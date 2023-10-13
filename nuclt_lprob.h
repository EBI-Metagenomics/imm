#ifndef IMM_NUCLT_LPROB_H
#define IMM_NUCLT_LPROB_H

#include "compiler.h"
#include "nuclt.h"
#include <stdio.h>

struct imm_nuclt_lprob
{
  struct imm_nuclt const *nuclt;
  float lprobs[IMM_NUCLT_SIZE];
};

struct lip_file;

IMM_API struct imm_nuclt_lprob imm_nuclt_lprob(struct imm_nuclt const *,
                                               float const *lprobs);
IMM_API float imm_nuclt_lprob_get(struct imm_nuclt_lprob const *, char symbol);
IMM_API int imm_nuclt_lprob_pack(struct imm_nuclt_lprob const *,
                                 struct lip_file *);
IMM_API int imm_nuclt_lprob_unpack(struct imm_nuclt_lprob *, struct lip_file *);
IMM_API void imm_nuclt_lprob_dump(struct imm_nuclt_lprob const *,
                                  FILE *restrict);

float imm__nuclt_lprob_get(struct imm_nuclt_lprob const *, int idx);

#endif
