#ifndef NORMAL_STATE_H
#define NORMAL_STATE_H

#include <stdio.h>

struct imm_state;

int normal_state_read(FILE* stream, struct imm_state* state);

#endif
