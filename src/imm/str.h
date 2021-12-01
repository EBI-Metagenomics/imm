#ifndef IMM_STR_H
#define IMM_STR_H

#include <string.h>

struct imm_str
{
    unsigned len;
    char const *data;
};

#define IMM_STR(str)                                                           \
    (struct imm_str) { sizeof(str) - 1, (str) }

static inline struct imm_str imm_str(char const data[static 1])
{
    return (struct imm_str){(unsigned)strlen(data), data};
}

#endif
