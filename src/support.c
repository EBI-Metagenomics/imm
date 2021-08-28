#include "support.h"
#include "imm/rc.h"
#include "imm/support.h"
#include "log.h"
#include <stdio.h>

void __imm_bug(char const *file, int line, char const *cond)
{
    fprintf(stderr, "BUG: %s:%d: %s\n", file, line, cond);
    fflush(stderr);
    exit(EXIT_FAILURE);
}
