#include "imm/bug.h"
#include <stdio.h>
#include <stdlib.h>

void imm_bug(char const *file, char const *func, int line, char const *cond)
{
    fprintf(stderr, "BUG: %s: %s: %d: %s\n", file, func, line, cond);
    fflush(stderr);
    exit(EXIT_FAILURE);
}
