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
struct imm_hmm_block;
struct imm_state;
struct model_state;

struct imm_dp* dp_create(struct imm_abc const* abc, struct model_state const** mstates, uint16_t nstates,
                         struct imm_state const* end_state);
void           dp_create_from_hmm_block(struct imm_hmm_block* hmm_block);
struct dp_emission const*    dp_get_emission(struct imm_dp const* dp);
struct model_state const* const*  dp_get_mstates(struct imm_dp const* dp);
struct seq_code const*       dp_get_seq_code(struct imm_dp const* dp);
struct dp_state_table const* dp_get_state_table(struct imm_dp const* dp);
struct dp_trans_table*       dp_get_trans_table(struct imm_dp const* dp);

#endif
