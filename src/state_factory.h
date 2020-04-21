#ifndef STATE_FACTORY_H
#define STATE_FACTORY_H

#include <inttypes.h>
#include <stdio.h>

struct imm_abc;
struct imm_state;

struct imm_state const* state_factory_read(FILE* stream, uint8_t type_id,
                                           struct imm_abc const* abc);

#endif
