#include "src/imm/ascii.h"
#include "imm.h"

int ascii_is_std(char const* str, size_t len)
{
    for (size_t i = 0; i < len; ++i) {
        if (str[i] < 0 || str[i] > 127)
            return 0;
    }
    return 1;
}
