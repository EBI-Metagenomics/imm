#ifndef ABC_H
#define ABC_H

#include <stdio.h>

struct imm_abc;
struct imm_io;

int abc_write(FILE* stream, struct imm_abc const* abc);
int abc_read(FILE* stream, struct imm_io* io);

#endif
