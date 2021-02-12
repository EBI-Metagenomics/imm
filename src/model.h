#ifndef MODEL_H
#define MODEL_H

#include "imm/model.h"
#include "imm/vec.h"
#include <inttypes.h>

struct imm_abc;
struct model_state;

struct imm_model
{
    struct imm_abc const* abc;
    struct imm_vecp*      hmm_blocks;

    imm_model_read_state_cb read_state;
    void*                   read_state_args;

    imm_model_write_state_cb write_state;
    void*                    write_state_args;
};

int write_mstates(struct imm_model const* model, FILE* stream,
                  struct model_state const* const* mstates, uint16_t nstates);

#endif
