#ifndef IO_ABC_H
#define IO_ABC_H

#include <stdio.h>

struct imm_abc;

int io_abc_write(FILE* stream, struct imm_abc const* abc);

#endif
