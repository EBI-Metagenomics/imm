#ifndef MUTE_STATE_H
#define MUTE_STATE_H

#include <stdio.h>

struct imm_abc;
struct imm_state;

struct imm_state const* mute_state_read(FILE* stream, struct imm_abc const* abc);

#endif
