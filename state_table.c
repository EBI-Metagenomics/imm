#include "state_table.h"
#include "assume.h"
#include "dp.h"
#include "dp_cfg.h"
#include "dump.h"
#include "fmt.h"
#include "lprob.h"
#include "rc.h"
#include "reallocf.h"
#include "state.h"
#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

void imm_state_table_init(struct imm_state_table *x)
{
  x->nstates = IMM_NSTATES_NULL;
  x->ids = NULL;
  x->start_state_idx = IMM_STATE_NULL_IDX;
  x->end_state_idx = IMM_STATE_NULL_IDX;
  x->span = NULL;
  x->debug.state_name = &imm_state_default_name;
}

void imm_state_table_debug_setup(struct imm_state_table *x,
                                 imm_state_name *callb)
{
  x->debug.state_name = callb;
}

void imm_state_table_cleanup(struct imm_state_table *x)
{
  if (x)
  {
    free(x->ids);
    x->ids = NULL;
    free(x->span);
    x->span = NULL;
    x->debug.state_name = NULL;
  }
}

int imm_state_table_reset(struct imm_state_table *x,
                          struct imm_dp_cfg const *cfg)
{
  assert(cfg->nstates > 0);
  x->nstates = cfg->nstates;
  x->ids = imm_reallocf(x->ids, sizeof(*x->ids) * (size_t)cfg->nstates);
  x->span = imm_reallocf(x->span, sizeof(*x->span) * (size_t)cfg->nstates);

  if (cfg->nstates > 0)
  {
    if (!x->ids || !x->span) return IMM_ENOMEM;
  }

  for (int i = 0; i < cfg->nstates; ++i)
  {
    x->ids[i] = (uint16_t)imm_state_id(cfg->states[i]);
    x->span[i] = imm_zspan(imm_state_span(cfg->states[i]).min,
                           imm_state_span(cfg->states[i]).max);
  }

  x->start_state_idx = (int16_t)cfg->start_state->idx;
  x->end_state_idx = cfg->end_state->idx;
  return 0;
}

int imm_state_table_idx(struct imm_state_table const *x, int state_id)
{
  for (int idx = 0; idx < x->nstates; ++idx)
  {
    if (x->ids[idx] == state_id) return idx;
  }
  return INT_MAX;
}

char *imm_state_table_name(struct imm_state_table const *x, int idx)
{
  static char name[IMM_STATE_NAME_SIZE] = {0};
  if (idx == IMM_STATE_NULL_IDX) return strcpy(name, "?");
  return (*x->debug.state_name)(imm_state_table_id(x, idx), name);
}

int imm_state_table_id(struct imm_state_table const *x, int idx)
{
  return x->ids[idx];
}

struct imm_range imm_state_table_range(struct imm_state_table const *x,
                                       int state_idx)
{
  return imm_zspan_range(imm_state_table_zspan(x, state_idx));
}

void imm_state_table_dump(struct imm_state_table const *x, FILE *restrict fp)
{
  fprintf(fp, "\n");
  fprintf(fp, "start_state=%s\n", imm_state_table_name(x, x->start_state_idx));
  fprintf(fp, "\n");

  fprintf(fp, "end_state=%s\n", imm_state_table_name(x, x->end_state_idx));
  fprintf(fp, "\n");

  for (int i = 0; i < x->nstates; ++i)
  {
    struct imm_range range = imm_zspan_range(imm_state_table_zspan(x, i));
    fprintf(fp, "%s=", imm_state_table_name(x, i));
    imm_range_dump(range, fp);
    fprintf(fp, "\n");
  }
}
