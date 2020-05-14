#ifndef TABLE_STATE_H
#define TABLE_STATE_H

#include <stdio.h>

struct imm_model;
struct imm_state;

int table_state_write(struct imm_state const* state, struct imm_model const* model, FILE* stream);

#endif
