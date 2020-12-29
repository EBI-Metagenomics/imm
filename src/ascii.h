#ifndef ASCII_H
#define ASCII_H

#include <stddef.h>

#define ASCII_LAST_STD 127

static inline int ascii_is_std(char const* str, size_t len)
{
    for (size_t i = 0; i < len; ++i) {
        signed int const a = str[i];
        if (a < 0)
            return 0;
    }
    return 1;
}

#endif
