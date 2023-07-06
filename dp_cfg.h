#ifndef IMM_DP_CFG_H
#define IMM_DP_CFG_H

struct imm_dp_cfg
{
  unsigned ntrans;
  unsigned nstates;
  struct imm_state **states;
  struct
  {
    struct imm_state const *state;
    float lprob;
  } start;
  struct imm_state const *end_state;
};

#endif
