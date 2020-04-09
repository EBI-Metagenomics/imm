#ifndef DP_H
#define DP_H

#include <stdio.h>

struct imm_abc;
struct mstate;
struct imm_state;
struct imm_dp;

struct imm_dp const* dp_create(struct imm_abc const* abc, struct mstate const* const* mstates,
                               unsigned const nstates, struct imm_state const* end_state);
int                  dp_write(struct imm_dp const* dp, FILE* stream);

#endif
