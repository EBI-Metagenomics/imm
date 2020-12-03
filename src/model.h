#ifndef MODEL_H
#define MODEL_H

#include "imm/model.h"
#include <inttypes.h>

struct dp_emission;
struct dp_state_table;
struct imm_abc;
struct imm_hmm;
struct imm_hmm_block;
struct mstate;
struct seq_code;

struct imm_model
{
    struct imm_abc const* abc;

    uint8_t                nhmm_blocks;
    struct imm_hmm_block** hmm_blocks;

    imm_hmm_block_read_state_cb read_state;
    void*                       read_state_args;

    imm_hmm_block_write_state_cb write_state;
    void*                        write_state_args;
};

#endif
