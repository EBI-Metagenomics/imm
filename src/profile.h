#ifndef PROFILE_H
#define PROFILE_H

#include "imm/imm.h"
#include <inttypes.h>

struct imm_abc;
struct imm_vecp;

struct imm_profile
{
    struct imm_abc const* abc;
    struct imm_vecp*      models;

    struct imm_profile_vtable vtable;
    void*                     derived;
};

#endif
