#ifndef IMM_PROD_H
#define IMM_PROD_H

#include "api.h"
#include "path.h"

struct imm_seq;

struct imm_prod
{
  struct imm_seq const *seq;
  struct imm_path path;
  float loglik;
  uint64_t mseconds;
};

IMM_API struct imm_prod imm_prod(void);
IMM_API void imm_prod_cleanup(struct imm_prod *);
IMM_API void imm_prod_reset(struct imm_prod *);

#endif
