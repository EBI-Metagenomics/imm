#include "prod.h"
#include "lprob.h"

struct imm_prod imm_prod(void)
{
  return (struct imm_prod){NULL, imm_path(), imm_lprob_nan(), 0};
}

void imm_prod_cleanup(struct imm_prod *x)
{
  if (x)
  {
    imm_path_cleanup(&x->path);
    *x = imm_prod();
  }
}

void imm_prod_reset(struct imm_prod *x)
{
  imm_path_reset(&x->path);
  x->loglik = imm_lprob_nan();
  x->mseconds = 0;
}
