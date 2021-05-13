#ifndef ABC_H
#define ABC_H

#include "imm/abc.h"
#include <stdio.h>

int abc_init(struct imm_abc *abc, unsigned len, char const *symbols,
             char any_symbol, struct imm_abc_vtable vtable);
void abc_write(struct imm_abc const *abc, FILE *file);
void abc_read(struct imm_abc *abc, FILE *file);

#endif
