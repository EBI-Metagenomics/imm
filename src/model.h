#ifndef MODEL_H
#define MODEL_H

#include "imm/model.h"
#include <inttypes.h>

struct dp_emission;
struct dp_state_table;
struct imm_abc;
struct imm_hmm;
struct mstate;
struct seq_code;

struct imm_model
{
    struct imm_abc const*        abc;
    struct imm_hmm*              hmm;
    struct mstate**              mstates;
    uint32_t                     nstates;
    struct imm_state const**     states;
    struct seq_code const*       seq_code;
    struct dp_emission const*    emission;
    struct dp_trans_table const* trans_table;
    struct dp_state_table const* state_table;
    struct imm_dp const*         dp;

    imm_model_read_state_cb read_state;
    void*                   read_state_args;

    imm_model_write_state_cb write_state;
    void*                    write_state_args;
};

#endif
