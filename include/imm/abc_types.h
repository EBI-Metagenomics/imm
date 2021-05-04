#ifndef IMM_ABC_TYPES_H
#define IMM_ABC_TYPES_H

#include <stdint.h>

enum imm_abc_typeid
{
    IMM_NULL_ABC = 0,
    IMM_ABC = 1,
    IMM_AMINO = 2,
};

struct imm_abc;

struct imm_abc_vtable
{
    void (*del)(struct imm_abc const *abc);
    enum imm_abc_typeid typeid;
    void *derived;
};

#endif
