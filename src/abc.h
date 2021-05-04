#ifndef ABC_H
#define ABC_H

#include "imm/abc.h"

int abc_init(struct imm_abc *abc, unsigned len, char const *symbols,
             char any_symbol, struct imm_abc_vtable vtable);

#endif
