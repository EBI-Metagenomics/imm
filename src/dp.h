#ifndef DP_H
#define DP_H

#include "imm/float.h"
#include <stdint.h>
#include <stdio.h>

struct dp_emission;
struct dp_state_table;
struct dp_trans_table;
struct imm_hmm;
struct imm_model;
struct imm_state;

struct imm_dp
{
    struct imm_state**           states;
    struct seq_code const*       seq_code;
    struct state_idx*            state_idx;
    struct dp_emission const*    emission;
    struct dp_trans_table*       trans_table;
    struct dp_state_table const* state_table;
};

struct imm_dp* dp_create(struct imm_hmm const* hmm, struct imm_state** states, struct imm_state const* end_state);
void           dp_create_from_model(struct imm_model* model);
struct dp_emission const*    dp_get_emission(struct imm_dp const* dp);
struct imm_state**           dp_get_states(struct imm_dp const* dp);
struct seq_code const*       dp_get_seq_code(struct imm_dp const* dp);
struct dp_state_table const* dp_get_state_table(struct imm_dp const* dp);
struct dp_trans_table*       dp_get_trans_table(struct imm_dp const* dp);

#endif
