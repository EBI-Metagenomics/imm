#ifndef IMM_STR_H
#define IMM_STR_H

struct imm_str
{
    unsigned size;
    char const *data;
};

#define IMM_STR(str)                                                           \
    (struct imm_str) { sizeof(str) - 1, (str) }

#endif
