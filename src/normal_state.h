#ifndef NORMAL_STATE_H
#define NORMAL_STATE_H

#include <stdio.h>

struct imm_abc;
struct imm_state;

struct imm_state const* normal_state_read(FILE* stream, struct imm_abc const* abc);

#endif
