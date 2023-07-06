#include "state_table.h"
#include "assume.h"
#include "dp.h"
#include "dp_cfg.h"
#include "dump.h"
#include "fmt.h"
#include "lprob.h"
#include "printer.h"
#include "rc.h"
#include "reallocf.h"
#include "state.h"
#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdlib.h>

void imm_state_table_init(struct imm_state_table *x)
{
  x->nstates = IMM_NSTATES_NULL;
  x->ids = NULL;
  x->start.state_idx = IMM_STATE_NULL_IDX;
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
  x->start.state_idx = (uint16_t)cfg->start.state->idx;
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
                                       unsigned state_idx)
{
  return imm_zspan_range(imm_state_table_zspan(x, state_idx));
}

void imm_state_table_dump(struct imm_state_table const *x,
                          imm_state_name *callb, FILE *restrict fp)
{
  char state_name[IMM_STATE_NAME_SIZE] = {0};

  (*callb)(imm_state_table_id(x, x->start.state_idx), state_name);
  char const *fmt32 = imm_printer_get_f32fmt();

  fprintf(fp, "\n");
  fprintf(fp, "start_state=%s\n", state_name);
  fprintf(fp, "start_lprob=");
  fprintf(fp, fmt32, x->start.lprob);
  fprintf(fp, "\n");

  (*callb)(imm_state_table_id(x, x->end_state_idx), state_name);
  fprintf(fp, "end_state=%s\n", state_name);
  fprintf(fp, "\n");

  for (unsigned i = 0; i < x->nstates; ++i)
  {
    struct imm_range range = imm_zspan_range(imm_state_table_zspan(x, i));
    (*callb)(imm_state_table_id(x, i), state_name);
    fprintf(fp, "%s=", state_name);
    imm_range_dump(range, fp);
    fprintf(fp, "\n");
  }
}
