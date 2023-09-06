#include "trans_table.h"
#include "defer_return.h"
#include "dp_cfg.h"
#include "dump.h"
#include "fmt.h"
#include "htable.h"
#include "list.h"
#include "lprob.h"
#include "rc.h"
#include "reallocf.h"
#include "state_table.h"
#include "trans.h"
#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdlib.h>

void imm_trans_table_init(struct imm_trans_table *x)
{
  x->ntrans = UINT_MAX;
  x->trans = NULL;
  x->offset = NULL;
}

int imm_trans_table_reset(struct imm_trans_table *x,
                          struct imm_dp_cfg const *cfg)
{
  int rc = 0;
  assert(cfg->nstates > 0);
  x->ntrans = cfg->ntrans;

  unsigned transsize = imm_trans_table_transsize(x->ntrans);
  x->trans = imm_reallocf(x->trans, sizeof(*x->trans) * transsize);
  if (!x->trans) defer_return(IMM_ENOMEM);

  unsigned offsize = imm_trans_table_offsize(cfg->nstates);
  x->offset = imm_reallocf(x->offset, sizeof(*x->offset) * offsize);
  if (!x->offset) defer_return(IMM_ENOMEM);

  unsigned offset = 0;
  x->offset[0] = offset;
  for (unsigned i = 0; i < cfg->nstates; ++i)
  {
    struct imm_trans *trans = NULL;
    unsigned j = 0;
    imm_list_for_each_entry(trans, &cfg->states[i]->trans.incoming, incoming)
    {
      x->trans[offset + j].score = trans->lprob;
      x->trans[offset + j].src = trans->pair.idx.src;
      x->trans[offset + j].dst = trans->pair.idx.dst;
      j++;
    }
    offset += j;
    x->offset[i + 1] = (uint16_t)offset;
  }
  x->trans[offset].score = imm_lprob_nan();
  x->trans[offset].src = UINT16_MAX;
  x->trans[offset].dst = UINT16_MAX;

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

unsigned imm_trans_table_idx(struct imm_trans_table const *x, unsigned src,
                             unsigned dst)
{
  for (unsigned i = 0; i < imm_trans_table_ntrans(x, dst); ++i)
  {
    if (imm_trans_table_source_state(x, dst, i) == src)
      return x->offset[dst] + i;
  }
  return IMM_TRANS_NULL_IDX;
}

void imm_trans_table_change(struct imm_trans_table *x, unsigned trans,
                            float score)
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

unsigned imm_trans_table_transsize(unsigned ntrans) { return ntrans + 1; }

unsigned imm_trans_table_offsize(unsigned nstates) { return nstates + 1; }

void imm_trans_table_dump(struct imm_trans_table const *x,
                          struct imm_state_table const *st, FILE *restrict fp)
{
  for (unsigned i = 0; i < x->ntrans; ++i)
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
