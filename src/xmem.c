#include "xmem.h"

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
