#ifndef XMEM_H
#define XMEM_H

#include "imm/support.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>

static inline void *xmalloc(size_t size)
{
    void *ptr = malloc(size);
    if (!ptr && size > 0)
        fatal(IMM_IOERROR, "failed to malloc");
    return ptr;
}

static inline void *xmemcpy(void *restrict dest, const void *restrict src,
                            size_t count)
{
    void *ptr = memcpy(dest, src, count);
    if (!ptr)
        fatal(IMM_IOERROR, "failed to memcpy");
    return ptr;
}

static inline void *xrealloc(void *ptr, size_t new_size)
{
    void *new_ptr = realloc(ptr, new_size);
    if (!new_ptr && new_size > 0)
        fatal(IMM_IOERROR, "failed to realloc");
    return new_ptr;
}

static inline void *xgrowmem(void *restrict ptr, size_t count, size_t size,
                             size_t *capacity)
{
    if (size * count > *capacity)
    {
        (*capacity) <<= 1;
        IMM_BUG(*capacity < size * count);
        ptr = xrealloc(ptr, *capacity);
    }
    return ptr;
}

#endif
