#ifndef DP_H
#define DP_H

#include <inttypes.h>
#include <stdio.h>

struct imm_abc;
struct mstate;
struct imm_state;
struct imm_dp;
struct dp_trans;
struct dp_states;

struct imm_dp const* dp_create(struct imm_abc const* abc, struct mstate const* const* mstates,
                               unsigned const nstates, struct imm_state const* end_state);
int                  dp_write(struct imm_dp const* dp, FILE* stream);
struct mstate const* const* dp_mstates(struct imm_dp const* dp);
struct dp_states const*     dp_states(struct imm_dp const* dp);
struct dp_trans const*      dp_trans_table(struct imm_dp const* dp);

#endif
