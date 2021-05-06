#ifndef IMM_TRIPLET_H
#define IMM_TRIPLET_H

#include "imm/sym.h"
#include <assert.h>

struct imm_triplet
{
    union
    {
        struct __attribute__((__packed__))
        {
            unsigned a;
            unsigned b;
            unsigned c;
        };
        unsigned ids[3];
    };
} __attribute__((__packed__));

static_assert(sizeof(struct imm_triplet) == 12, "imm_triplet must be packed");

#define IMM_TRIPLET(A, B, C)                                                   \
    (struct imm_triplet)                                                       \
    {                                                                          \
        .ids = { imm_sym_id(A), imm_sym_id(B), imm_sym_id(C) }                 \
    }

#endif
