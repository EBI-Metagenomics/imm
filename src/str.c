#include "imm/str.h"
#include <string.h>

struct imm_str imm_str(char const data[static 1])
{
    return (struct imm_str){(unsigned)strlen(data), data};
}
