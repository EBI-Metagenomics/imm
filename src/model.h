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

    struct imm_model_vtable vtable;
    void*                   derived;
};

int write_mstates(struct imm_model const* model, FILE* stream,
                  struct model_state const* const* mstates, uint16_t nstates);

#endif
