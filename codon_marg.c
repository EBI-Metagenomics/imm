#include "codon_marg.h"
#include "abc.h"
#include "codon_lprob.h"
#include "dump.h"
#include "lite_pack.h"
#include "lite_pack_io.h"
#include "lprob.h"
#include "nuclt.h"
#include "rc.h"
#include "static_assert.h"
#include <assert.h>

imm_static_assert(IMM_NUCLT_SIZE == 4, "nuclt size expected to be four");

struct codon_iter
{
  struct imm_nuclt const *nuclt;
  int pos;
};

static inline struct codon_iter codon_iter_begin(struct imm_nuclt const *nuclt)
{
  return (struct codon_iter){nuclt, 0};
}

static inline struct imm_codon codon_iter_next(struct codon_iter *iter)
{
  int n = IMM_NUCLT_SIZE;

  struct imm_codon codon = {.nuclt = iter->nuclt,
                            .a = (iter->pos / (n * n)) % n,
                            .b = (iter->pos / n) % n,
                            .c = iter->pos % n};
  iter->pos++;

  return codon;
}

static inline bool codon_iter_end(struct codon_iter const iter)
{
  int n = IMM_NUCLT_SIZE;
  return iter.pos >= n * n * n;
}

static float marginalization(struct imm_codon_marg const *codonm,
                             struct imm_codon const *codon)
{
  int const symbol_idx[IMM_NUCLT_SIZE] = {0, 1, 2, 3};
  int any = imm_abc_any_symbol_idx(&codonm->nuclt->super);
  int const *arr[3];
  int shape[3];
  for (int i = 0; i < 3; ++i)
  {
    if (codon->idx[i] == any)
    {
      arr[i] = symbol_idx;
      shape[i] = IMM_NUCLT_SIZE;
    }
    else
    {
      arr[i] = codon->idx + i;
      shape[i] = 1;
    }
  }

  struct imm_codon t;
  t.nuclt = codon->nuclt;
  float lprob = imm_lprob_zero();
  for (int a = 0; a < shape[0]; ++a)
  {
    for (int b = 0; b < shape[1]; ++b)
    {
      for (int c = 0; c < shape[2]; ++c)
      {
        t.a = arr[0][a];
        t.b = arr[1][b];
        t.c = arr[2][c];
        lprob = imm_lprob_add(lprob, imm_codon_marg_lprob(codonm, t));
      }
    }
  }

  return lprob;
}

static void set_marginal_lprobs(struct imm_codon_marg *codonm)
{
  struct imm_abc const *abc = &codonm->nuclt->super;
  int any = imm_abc_any_symbol_idx(abc);
  assert(any == imm_nuclt_size(codonm->nuclt));

  struct imm_codon codon;
  codon.nuclt = codonm->nuclt;

  int size = IMM_NUCLT_SIZE + 1;
  for (int k = 0; k < 3; ++k)
  {
    for (int i = 0; i < size; ++i)
    {
      for (int j = 0; j < size; ++j)
      {
        codon.idx[k] = any;
        codon.idx[(k + 1) % 3] = i;
        codon.idx[(k + 2) % 3] = j;
        float lprob = marginalization(codonm, &codon);
        codonm->lprobs[codon.idx[0]][codon.idx[1]][codon.idx[2]] = lprob;
      }
    }
  }
}

static void set_nonmarginal_lprobs(struct imm_codon_marg *codonm,
                                   struct imm_codon_lprob const *codonp)
{
  struct codon_iter iter = codon_iter_begin(codonm->nuclt);
  while (!codon_iter_end(iter))
  {
    struct imm_codon const codon = codon_iter_next(&iter);
    float lprob = imm_codon_lprob_get(codonp, codon);
    codonm->lprobs[codon.idx[0]][codon.idx[1]][codon.idx[2]] = lprob;
  }
}

struct imm_codon_marg imm_codon_marg(struct imm_codon_lprob *codonp)
{
  struct imm_codon_marg codonm;
  codonm.nuclt = codonp->nuclt;
  set_nonmarginal_lprobs(&codonm, codonp);
  set_marginal_lprobs(&codonm);
  return codonm;
}

#define CODON_SIZE                                                             \
  ((IMM_NUCLT_SIZE + 1) * (IMM_NUCLT_SIZE + 1) * (IMM_NUCLT_SIZE + 1))

int imm_codon_marg_pack(struct imm_codon_marg const *codonm,
                        struct lio_writer *f)
{
  if (lio_write(f, lip_pack_array(lio_alloc(f), CODON_SIZE))) return IMM_EIO;
  for (int i = 0; i < CODON_SIZE; ++i)
    if (lio_write(f, lip_pack_float(lio_alloc(f), codonm->lprobs[0][0][i]))) return IMM_EIO;

  return 0;
}

int imm_codon_marg_unpack(struct imm_codon_marg *codonm, struct lio_reader *f)
{
  uint32_t u32 = 0;
  if (lio_free(f, lip_unpack_array(lio_read(f), &u32))) return IMM_EIO;
  if (u32 != (uint32_t)CODON_SIZE) return IMM_EIO;
  for (int i = 0; i < CODON_SIZE; ++i)
    if (lio_free(f, lip_unpack_float(lio_read(f), codonm->lprobs[0][0] + i))) return IMM_EIO;

  return 0;
}

void imm_codon_marg_dump(struct imm_codon_marg const *x, FILE *restrict fp)
{
  size_t n = (IMM_NUCLT_SIZE + 1) * (IMM_NUCLT_SIZE + 1) * (IMM_NUCLT_SIZE + 1);
  imm_dump_array_f32(n, (float const *)x->lprobs, fp);
}
