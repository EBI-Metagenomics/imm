#ifndef PROFILE_H
#define PROFILE_H

#include "imm/profile.h"
#include "imm/vec.h"
#include <inttypes.h>

struct imm_abc;
struct model_state;

struct imm_profile
{
    struct imm_abc const* abc;
    struct imm_vecp*      hmm_blocks;

    struct imm_profile_vtable vtable;
    void*                     derived;
};

int write_mstates(struct imm_profile const* prof, FILE* stream, struct model_state const* const* mstates,
                  uint16_t nstates);

#endif
