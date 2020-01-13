#include "imm/imm.h"
#include <stdio.h>

void imm_error(char const* err, ...)
{
    va_list params;
    va_start(params, err);
    vfprintf(stderr, err, params);
    fputc('\n', stderr);
    va_end(params);
}
