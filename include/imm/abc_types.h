#ifndef IMM_ABC_TYPES_H
#define IMM_ABC_TYPES_H

#include <stdint.h>

#define IMM_ABC_TID 0x00
#define IMM_ABC_NULL_TID UINT8_MAX

typedef uint8_t imm_abc_tid_t;

enum abc_typeid
{
    IMM_ABC = 0,
    IMM_AMINO = 1,
    IMM_NULL_ABC = IMM_ABC_NULL_TID,
};

#endif
