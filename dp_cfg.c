#include "dp_cfg.h"

void imm_dp_cfg_set_ntrans(struct imm_dp_cfg *x, unsigned ntrans)
{
  x->ntrans = ntrans;
}

void imm_dp_cfg_set_states(struct imm_dp_cfg *x, unsigned nstates,
                           struct imm_state **states)
{
  x->nstates = nstates;
  x->states = states;
}

void imm_dp_cfg_set_start(struct imm_dp_cfg *x, struct imm_state const *start,
                          float lprob)
{
  x->start.state = start;
  x->start.lprob = lprob;
}

void imm_dp_cfg_set_end(struct imm_dp_cfg *x, struct imm_state const *end)
{
  x->end_state = end;
}
