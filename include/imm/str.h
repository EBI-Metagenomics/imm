#ifndef IMM_STR_H
#define IMM_STR_H

struct imm_str
{
    unsigned len;
    char const *data;
};

#define IMM_STR(str)                                                           \
    (struct imm_str) { sizeof(str) - 1, (str) }

static inline void imm_str(struct imm_str *str, unsigned len, char const *data)
{
    str->len = len;
    str->data = data;
}

#endif
