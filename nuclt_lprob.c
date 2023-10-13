#include "nuclt_lprob.h"
#include "dump.h"
#include "expect.h"
#include "lip/1darray/1darray.h"
#include "lip/lip.h"
#include "rc.h"

struct imm_nuclt_lprob imm_nuclt_lprob(struct imm_nuclt const *nuclt,
                                       float const *lprobs)
{
  struct imm_nuclt_lprob lprob;
  lprob.nuclt = nuclt;
  for (int i = 0; i < IMM_NUCLT_SIZE; ++i)
    lprob.lprobs[i] = lprobs[i];
  return lprob;
}

float imm_nuclt_lprob_get(struct imm_nuclt_lprob const *lprob, char symbol)
{
  struct imm_abc const *abc = &lprob->nuclt->super;
  assert(imm_abc_has_symbol(abc, symbol));
  return imm__nuclt_lprob_get(lprob, imm_abc_symbol_idx(abc, symbol));
}

int imm_nuclt_lprob_pack(struct imm_nuclt_lprob const *nucltp,
                         struct lip_file *file)
{
  lip_write_1darray_float(file, IMM_NUCLT_SIZE, nucltp->lprobs);

  return file->error ? IMM_EIO : 0;
}

int imm_nuclt_lprob_unpack(struct imm_nuclt_lprob *nucltp,
                           struct lip_file *file)
{
  imm_expect_1darr_float_type(file, IMM_NUCLT_SIZE, nucltp->lprobs);

  return file->error ? IMM_EIO : 0;
}

void imm_nuclt_lprob_dump(struct imm_nuclt_lprob const *x, FILE *restrict fp)
{
  imm_dump_array_f32(IMM_NUCLT_SIZE, x->lprobs, fp);
}

float imm__nuclt_lprob_get(struct imm_nuclt_lprob const *lprob, int idx)
{
  return lprob->lprobs[idx];
}
