#ifndef COMMON_MEMORY_H
#define COMMON_MEMORY_H

#include "common/bug.h"
#include "common/error.h"
#include <stdlib.h>
#include <string.h>

#define xmalloc(x) __malloc((x), __FILE__, __LINE__)
#define xmemcpy(d, s, c) __memcpy((d), (s), (c), __FILE__, __LINE__)
#define xrealloc(ptr, new_size) __realloc((ptr), (new_size), __FILE__, __LINE__)
#define xstrdup(x) __strdup((x), __FILE__, __LINE__)
#define growmem(ptr, count, size, capacity)                                    \
    __growmem((ptr), (count), (size), (capacity), __FILE__, __LINE__)

static inline void free_if(void const *_Nullable ptr)
{
    if (ptr)
        free((void *)ptr);
}

static inline void *_Nonnull __malloc(size_t size, char const *_Nonnull file,
                                      int line)
{
    void *ptr = malloc(size);
    if (!ptr)
    {
        __imm_log(IMM_LOG_FATAL, file, line, "failed to malloc");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

static inline void *_Nonnull __memcpy(void *restrict _Nonnull dest,
                                      const void *restrict _Nonnull src,
                                      size_t count, char const *_Nonnull file,
                                      int line)
{
    void *ptr = memcpy(dest, src, count);
    if (!ptr)
    {
        __imm_log(IMM_LOG_FATAL, file, line, "failed to memcpy");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

static inline void *_Nonnull __realloc(void *_Nonnull ptr, size_t new_size,
                                       char const *_Nonnull file, int line)
{
    void *new_ptr = realloc(ptr, new_size);
    if (!new_ptr)
    {
        __imm_log(IMM_LOG_FATAL, file, line, "failed to realloc");
        exit(EXIT_FAILURE);
    }
    return new_ptr;
}

static inline char *_Nonnull __strdup(char const *_Nonnull str,
                                      char const *_Nonnull file, int line)
{
    char *new = strdup(str);
    if (!new)
    {
        __imm_log(IMM_LOG_FATAL, file, line, "failed to strdup");
        exit(EXIT_FAILURE);
    }
    return new;
}

static inline void *_Nonnull __growmem(void *restrict _Nonnull ptr,
                                       size_t count, size_t size,
                                       size_t *_Nonnull capacity,
                                       char const *_Nonnull file, int line)
{
    if (size * count > *capacity)
    {
        (*capacity) <<= 1;
        BUG(*capacity < size * count);
        ptr = __realloc(ptr, *capacity, file, line);
    }
    return ptr;
}

#endif
