#ifndef DP_H
#define DP_H

#include <inttypes.h>
#include <stdio.h>

struct dp_emission;
struct dp_state_table;
struct dp_trans_table;
struct imm_abc;
struct imm_dp;
struct imm_io;
struct imm_state;
struct mstate;

struct imm_dp const* dp_create(struct imm_abc const* abc, struct mstate const* const* mstates,
                               uint32_t nstates, struct imm_state const* end_state);
int                  dp_write(struct imm_dp const* dp, FILE* stream);
int                  dp_read(FILE* stream, struct imm_io* io);
struct mstate const* const*  dp_get_mstates(struct imm_dp const* dp);
struct dp_state_table const* dp_get_state_table(struct imm_dp const* dp);
struct dp_trans_table const* dp_get_trans_table(struct imm_dp const* dp);
struct dp_emission const*    dp_get_emission(struct imm_dp const* dp);

#endif
