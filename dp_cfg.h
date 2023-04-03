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

void imm_dp_cfg_set_ntrans(struct imm_dp_cfg *, unsigned ntrans);
void imm_dp_cfg_set_states(struct imm_dp_cfg *, unsigned nstates,
                           struct imm_state **states);
void imm_dp_cfg_set_start(struct imm_dp_cfg *, struct imm_state const *,
                          float lprob);
void imm_dp_cfg_set_end(struct imm_dp_cfg *, struct imm_state const *);

#endif
