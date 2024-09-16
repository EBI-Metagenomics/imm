#include "imm_trans_table.h"
#include "defer_return.h"
#include "imm_dp_cfg.h"
#include "imm_fmt.h"
#include "imm_htable.h"
#include "imm_list.h"
#include "imm_lprob.h"
#include "imm_rc.h"
#include "imm_reallocf.h"
#include "imm_state_table.h"
#include "imm_trans.h"
#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdlib.h>

void imm_trans_table_init(struct imm_trans_table *x)
{
  x->ntrans = INT_MAX;
  x->trans = NULL;
  x->offset = NULL;
}

int imm_trans_table_reset(struct imm_trans_table *x,
                          struct imm_dp_cfg const *cfg)
{
  int rc = 0;
  assert(cfg->nstates > 0);
  x->ntrans = cfg->ntrans;

  size_t transsize = (size_t)imm_trans_table_transsize(x->ntrans);
  x->trans = imm_reallocf(x->trans, sizeof(*x->trans) * transsize);
  if (!x->trans) defer_return(IMM_ENOMEM);

  size_t offsize = (size_t)imm_trans_table_offsize(cfg->nstates);
  x->offset = imm_reallocf(x->offset, sizeof(*x->offset) * offsize);
  if (!x->offset) defer_return(IMM_ENOMEM);

  int offset = 0;
  x->offset[0] = (int16_t)offset;
  for (int i = 0; i < cfg->nstates; ++i)
  {
    struct imm_trans *trans = NULL;
    int j = 0;
    imm_list_for_each_entry(trans, &cfg->states[i]->trans.incoming, incoming)
    {
      x->trans[offset + j].score = trans->lprob;
      x->trans[offset + j].src = trans->pair.idx.src;
      x->trans[offset + j].dst = trans->pair.idx.dst;
      j++;
    }
    offset += j;
    x->offset[i + 1] = (int16_t)offset;
  }
  x->trans[offset].score = imm_lprob_nan();
  x->trans[offset].src = INT16_MAX;
  x->trans[offset].dst = INT16_MAX;

  return rc;

defer:
  if (x->trans)
  {
    free(x->trans);
    x->trans = NULL;
  }
  if (x->offset)
  {
    free(x->offset);
    x->offset = NULL;
  }
  return rc;
}

int imm_trans_table_idx(struct imm_trans_table const *x, int src, int dst)
{
  for (int i = 0; i < imm_trans_table_ntrans(x, dst); ++i)
  {
    if (imm_trans_table_source_state(x, dst, i) == src)
      return x->offset[dst] + i;
  }
  return IMM_TRANS_NULL_IDX;
}

void imm_trans_table_change(struct imm_trans_table *x, int trans, float score)
{
  x->trans[trans].score = score;
}

void imm_trans_table_cleanup(struct imm_trans_table *x)
{
  if (x)
  {
    free(x->trans);
    x->trans = NULL;
    free(x->offset);
    x->offset = NULL;
  }
}

int imm_trans_table_transsize(int ntrans) { return ntrans + 1; }

int imm_trans_table_offsize(int nstates) { return nstates + 1; }

void imm_trans_table_dump(struct imm_trans_table const *x,
                          struct imm_state_table const *st, FILE *restrict fp)
{
  for (int i = 0; i < x->ntrans; ++i)
  {
    fputs(imm_state_table_name(st, x->trans[i].src), fp);
    fputs(" -> ", fp);
    fputs(imm_state_table_name(st, x->trans[i].dst), fp);
    fputs(" [", fp);
    fprintf(fp, imm_fmt_get_f32(), x->trans[i].score);
    fputs("]\n", fp);
  }
  fprintf(fp, "\n");
}
