#include "imm_nuclt_lprob.h"
#include "imm_dump.h"
#include "imm_rc.h"
#include "lite_pack.h"
#include "lite_pack_io.h"
#include <assert.h>

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
                         struct lio_writer *f)
{
  if (lio_write(f, lip_pack_array(lio_alloc(f), IMM_NUCLT_SIZE))) return IMM_EIO;
  for (int i = 0; i < IMM_NUCLT_SIZE; ++i)
    if (lio_write(f, lip_pack_float(lio_alloc(f), nucltp->lprobs[i]))) return IMM_EIO;

  return 0;
}

int imm_nuclt_lprob_unpack(struct imm_nuclt_lprob *nucltp,
                           struct lio_reader *f)
{
  uint32_t u32 = 0;
  if (lio_free(f, lip_unpack_array(lio_read(f), &u32))) return IMM_EIO;
  if (u32 != (uint32_t)IMM_NUCLT_SIZE) return IMM_EIO;
  for (int i = 0; i < IMM_NUCLT_SIZE; ++i)
    if (lio_free(f, lip_unpack_float(lio_read(f), nucltp->lprobs + i))) return IMM_EIO;

  return 0;
}

void imm_nuclt_lprob_dump(struct imm_nuclt_lprob const *x, FILE *restrict fp)
{
  imm_dump_array_f32(IMM_NUCLT_SIZE, x->lprobs, fp);
}

float imm__nuclt_lprob_get(struct imm_nuclt_lprob const *lprob, int idx)
{
  return lprob->lprobs[idx];
}
