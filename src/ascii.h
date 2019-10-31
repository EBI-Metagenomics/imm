#ifndef IMM_ASCII_H
#define IMM_ASCII_H

#include <stddef.h>

#define ASCII_LAST_STD 127

static inline int ascii_is_std(char const* str, size_t len)
{
    for (size_t i = 0; i < len; ++i) {
        if (str[i] < 0 || str[i] > 127)
            return 0;
    }
    return 1;
}

#endif
