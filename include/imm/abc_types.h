#ifndef IMM_ABC_TYPES_H
#define IMM_ABC_TYPES_H

#include "imm/export.h"
#include <stdbool.h>
#include <stdint.h>

#define IMM_ABC_TYPEID_SIZE 10

enum imm_abc_typeid
{
    IMM_NULL_ABC = 0,
    IMM_ABC = 1,
    IMM_AMINO = 2,
    IMM_NUCLT = 3,
    IMM_DNA = 4,
    IMM_RNA = 5,
};

static inline bool imm_abc_typeid_valid(unsigned typeid)
{
    return typeid > IMM_NULL_ABC && typeid <= IMM_RNA;
}

typedef uint8_t imm_abc_typeid_t;

struct imm_abc_vtable
{
    enum imm_abc_typeid typeid;
    void *derived;
};

IMM_API char const *imm_abc_typeid_name(enum imm_abc_typeid typeid);

#endif
