#include "std/bug.h"
#include <stdio.h>
#include <stdlib.h>

void std_bug_print_exit(char const* file, char const* func, int line, char const* cond)
{
    fprintf(stderr, "BUG: %s: %s: %d: %s\n", file, func, line, cond);
    fflush(stderr);
    exit(1);
}
