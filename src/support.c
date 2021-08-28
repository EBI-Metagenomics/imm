#include "support.h"
#include "imm/rc.h"
#include "imm/support.h"
#include "log.h"
#include <stdio.h>

#ifndef HAVE_REALLOFC
void *reallocf(void *ptr, size_t size)
{
    void *nptr;

    nptr = realloc(ptr, size);

    /*
     * When the System V compatibility option (malloc "V" flag) is
     * in effect, realloc(ptr, 0) frees the memory and returns NULL.
     * So, to avoid double free, call free() only when size != 0.
     * realloc(ptr, 0) can't fail when ptr != NULL.
     */
    if (!nptr && ptr && size != 0)
        free(ptr);
    return (nptr);
}
#endif

void __imm_bug(char const *file, int line, char const *cond)
{
    fprintf(stderr, "BUG: %s:%d: %s\n", file, line, cond);
    fflush(stderr);
    exit(EXIT_FAILURE);
}
