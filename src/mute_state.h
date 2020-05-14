#ifndef MUTE_STATE_H
#define MUTE_STATE_H

#include <stdio.h>

struct imm_abc;
struct imm_model;
struct imm_state;

struct imm_state const* imm_mute_state_read(FILE* stream, struct imm_abc const* abc);
int mute_state_write(struct imm_state const* state, struct imm_model const* model, FILE* stream);

#endif
