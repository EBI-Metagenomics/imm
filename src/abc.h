#ifndef ABC_H
#define ABC_H

#include "imm/abc.h"

static inline imm_abc_tid_t abc_typeid(struct imm_abc const *abc)
{
    return IMM_ABC_TYPE_ID;
}

void abc_del(struct imm_abc const *abc);

struct imm_abc *abc_new(unsigned len, char const *symbols, char any_symbol,
                        void *derived);

static inline void const *abc_derived(struct imm_abc const *abc)
{
    return abc->derived;
}

#endif
