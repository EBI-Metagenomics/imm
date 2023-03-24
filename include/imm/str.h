#ifndef IMM_STR_H
#define IMM_STR_H

#include "imm/export.h"

struct imm_str
{
    unsigned len;
    char const *data;
};

#define IMM_STR(str)                                                           \
    (struct imm_str) { sizeof(str) - 1, (str) }

IMM_API struct imm_str imm_str(char const data[static 1]);

#endif
