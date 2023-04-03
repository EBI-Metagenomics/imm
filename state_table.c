#include "state_table.h"
#include "assume.h"
#include "cspan.h"
#include "dp.h"
#include "dp_cfg.h"
#include "lprob.h"
#include "rc.h"
#include "reallocf.h"
#include "state.h"
#include <assert.h>
#include <limits.h>
#include <stdlib.h>

void imm_state_table_init(struct imm_state_table *x)
{
  x->nstates = IMM_NSTATES_NULL;
  x->ids = NULL;
  x->start.state = IMM_STATE_NULL_IDX;
  x->start.lprob = imm_lprob_nan();
  x->end_state_idx = IMM_STATE_NULL_IDX;
  x->span = NULL;
}

void imm_state_table_cleanup(struct imm_state_table *x)
{
  if (x)
  {
    free(x->ids);
    x->ids = NULL;
    free(x->span);
    x->span = NULL;
  }
}

int imm_state_table_reset(struct imm_state_table *x,
                          struct imm_dp_cfg const *cfg)
{
  x->nstates = cfg->nstates;
  x->ids = imm_reallocf(x->ids, sizeof(*x->ids) * cfg->nstates);
  x->span = imm_reallocf(x->span, sizeof(*x->span) * cfg->nstates);

  if (cfg->nstates > 0)
  {
    if (!x->ids || !x->span) return IMM_ENOMEM;
  }

  for (unsigned i = 0; i < cfg->nstates; ++i)
  {
    x->ids[i] = (uint16_t)imm_state_id(cfg->states[i]);
    x->span[i] = imm_zspan(imm_state_span(cfg->states[i]).min,
                           imm_state_span(cfg->states[i]).max);
  }

  x->start.lprob = cfg->start.lprob;
  x->start.state = (uint16_t)cfg->start.state->idx;
  x->end_state_idx = cfg->end_state->idx;
  return 0;
}

unsigned imm_state_table_idx(struct imm_state_table const *x, unsigned state_id)
{
  for (unsigned idx = 0; idx < x->nstates; ++idx)
  {
    if (x->ids[idx] == state_id) return idx;
  }
  return UINT_MAX;
}

unsigned imm_state_table_id(struct imm_state_table const *x, unsigned idx)
{
  return x->ids[idx];
}

struct imm_range imm_state_table_range(struct imm_state_table const *x,
                                       unsigned state)
{
  uint8_t span = imm_state_table_span(x, state);
  return imm_range(imm_zspan_min(span), imm_zspan_max(span) + 1);
}

void imm_state_table_dump(struct imm_state_table const *tbl)
{
  for (unsigned i = 0; i < tbl->nstates; ++i)
  {
    printf("%u", i);
    if (i + 1 < tbl->nstates) putc('\t', stdout);
  }
  putc('\n', stdout);

  for (unsigned i = 0; i < tbl->nstates; ++i)
  {
    printf("%u", tbl->ids[i]);
    if (i + 1 < tbl->nstates) putc('\t', stdout);
  }
  putc('\n', stdout);
}
