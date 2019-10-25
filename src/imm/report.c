#include "imm.h"
#include <stdio.h>

void imm_error(const char* err, ...)
{
    va_list params;
    va_start(params, err);
    fprintf(stderr, err, params);
    fputc('\n', stderr);
    va_end(params);
}
