#ifndef IMM_TRIPLET_H
#define IMM_TRIPLET_H

#include <assert.h>

struct imm_triplet
{
    union
    {
        struct __attribute__((__packed__))
        {
            char a;
            char b;
            char c;
            char pad;
        };
        char str[4];
    };
} __attribute__((__packed__));

static_assert(sizeof(struct imm_triplet) == 4, "imm_triplet must be packed");

#define IMM_TRIPLET_INIT(A, B, C)                                              \
    (struct imm_triplet)                                                       \
    {                                                                          \
        .str = { A, B, C, '\0' }                                               \
    }

#endif
