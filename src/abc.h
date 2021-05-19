#ifndef ABC_H
#define ABC_H

#include "imm/abc.h"

int abc_init(struct imm_abc *abc, unsigned len, char const *symbols,
             char any_symbol, struct imm_abc_vtable vtable);
int abc_write(struct imm_abc const *abc, FILE *file);
int abc_read(struct imm_abc *abc, FILE *file);

#endif
