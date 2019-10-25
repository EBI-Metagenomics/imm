#include "src/imm/ascii.h"
#include "imm.h"

int is_std_ascii(char const* str, size_t len)
{
    for (size_t i = 0; i < len; ++i) {
        if (str[i] < 0 || str[i] > 127)
            return 0;
    }
    return 1;
}
