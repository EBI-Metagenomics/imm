#ifndef IMM_ABC_TYPES_H
#define IMM_ABC_TYPES_H

#include <stdint.h>

enum imm_abc_typeid
{
    IMM_NULL_ABC = 0,
    IMM_ABC = 1,
    IMM_AMINO = 2,
    IMM_NUCLT = 3,
    IMM_DNA = 4,
    IMM_RNA = 5,
};

typedef uint8_t imm_abc_typeid_t;

struct imm_abc_vtable
{
    enum imm_abc_typeid typeid;
    void *derived;
};

#endif
