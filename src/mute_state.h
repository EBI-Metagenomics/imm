#ifndef MUTE_STATE_H
#define MUTE_STATE_H

#include <stdio.h>

struct imm_state;

int mute_state_read(FILE* stream, struct imm_state* state);

#endif
