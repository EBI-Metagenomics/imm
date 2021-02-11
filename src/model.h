#ifndef MODEL_H
#define MODEL_H

#include "imm/model.h"
#include "imm/vec.h"
#include <inttypes.h>

struct imm_abc;

struct imm_model
{
    struct imm_abc const* abc;
    struct imm_vecp*      hmm_blocks;

    imm_model_read_state_cb read_state;
    void*                   read_state_args;

    imm_model_write_state_cb write_state;
    void*                    write_state_args;
};

#endif
