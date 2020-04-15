#ifndef STATE_FACTORY_H
#define STATE_FACTORY_H

#include <stdio.h>
#include <inttypes.h>

struct imm_state;

int state_factory_read(FILE *stream, struct imm_state *state);


#endif
