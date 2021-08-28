#ifndef XMEM_H
#define XMEM_H

#include "bug.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>

static inline void *growmem(void *restrict ptr, size_t count, size_t size,
                            size_t *capacity)
{
    if (size * count > *capacity)
    {
        (*capacity) <<= 1;
        BUG(*capacity < size * count);
        ptr = realloc(ptr, *capacity);
    }
    return ptr;
}

#ifndef HAVE_REALLOFC
void *reallocf(void *ptr, size_t size);
#endif

#endif
