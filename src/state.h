#ifndef STATE_H
#define STATE_H

#include <stdio.h>

struct imm_state;
struct imm_abc;

int                     state_write(struct imm_state const* state, FILE* stream);
struct imm_state const* state_read(FILE* stream, struct imm_abc const* abc);

#endif
