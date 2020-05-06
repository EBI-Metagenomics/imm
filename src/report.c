#include "imm/imm.h"
#include <stdio.h>
#include <stdlib.h>

void imm_die(char const* err, ...)
{
    va_list params;
    va_start(params, err);
    imm_error(err, params);
    va_end(params);
    exit(1);
}

void imm_error(char const* err, ...)
{
    va_list params;
    va_start(params, err);
    vfprintf(stderr, err, params);
    fputc('\n', stderr);
    va_end(params);
}
