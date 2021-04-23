#ifndef COMMON_MEMORY_H
#define COMMON_MEMORY_H

#include "common/error.h"
#include <stdlib.h>
#include <string.h>

#define xmalloc(x) __malloc((x), __FILE__, __LINE__)
#define xmemcpy(d, s, c) __memcpy((d), (s), (c), __FILE__, __LINE__)
#define xrealloc(ptr, new_size) __realloc((ptr), (new_size), __FILE__, __LINE__)
#define xstrdup(x) __strdup((x), __FILE__, __LINE__)

static inline void free_if(void const *ptr)
{
    if (ptr)
        free((void *)ptr);
}

static inline void *__malloc(size_t size, char const *file, int line)
{
    void *ptr = malloc(size);
    if (!ptr)
    {
        __imm_log(IMM_LOG_FATAL, file, line, "failed to malloc");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

static inline void *__memcpy(void *restrict dest, const void *restrict src,
                             size_t count, char const *file, int line)
{
    void *ptr = memcpy(dest, src, count);
    if (!ptr)
    {
        __imm_log(IMM_LOG_FATAL, file, line, "failed to memcpy");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

static inline void *__realloc(void *ptr, size_t new_size, char const *file,
                              int line)
{
    void *new_ptr = realloc(ptr, new_size);
    if (!new_ptr)
    {
        __imm_log(IMM_LOG_FATAL, file, line, "failed to realloc");
        exit(EXIT_FAILURE);
    }
    return new_ptr;
}

static inline char *__strdup(char const *str, char const *file, int line)
{
    char *new = strdup(str);
    if (!new)
    {
        __imm_log(IMM_LOG_FATAL, file, line, "failed to strdup");
        exit(EXIT_FAILURE);
    }
    return new;
}

#endif
