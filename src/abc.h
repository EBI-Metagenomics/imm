#ifndef ABC_H
#define ABC_H

#include <stdio.h>

struct imm_abc;

int                   abc_write(FILE* stream, struct imm_abc const* abc);
struct imm_abc const* abc_read(FILE* stream);

#endif
