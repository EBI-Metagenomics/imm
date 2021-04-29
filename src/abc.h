#ifndef ABC_H
#define ABC_H

#include "imm/abc.h"

void abc_del(struct imm_abc const *abc);

struct imm_abc *abc_new(unsigned len, char const *symbols, char any_symbol,
                        struct imm_abc_vtable vtable);

#endif
