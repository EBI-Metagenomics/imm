#ifndef COMMON_MEMORY_H
#define COMMON_MEMORY_H

#include "common/error.h"
#include "imm/bug.h"
#include <stdlib.h>
#include <string.h>

#define xmalloc(x) __malloc((x), __FILE__, __LINE__)
#define xmemcpy(d, s, c) __memcpy((d), (s), (c), __FILE__, __LINE__)
#define xrealloc(ptr, new_size) __realloc((ptr), (new_size), __FILE__, __LINE__)
#define xstrdup(x) __strdup((x), __FILE__, __LINE__)
#define growmem(ptr, count, size, capacity)                                    \
    __growmem((ptr), (count), (size), (capacity), __FILE__, __LINE__)

static inline void *__memcpy(void *restrict dest, const void *restrict src,
                             size_t count, char const file[static 1], int line)
    __attribute__((nonnull(1, 2)));

static inline void *__realloc(void *ptr, size_t new_size,
                              char const file[static 1], int line)
    __attribute__((nonnull(1)));

static inline char *__strdup(char const *str, char const file[static 1],
                             int line) __attribute__((nonnull(1)));

static inline char *__strdup(char const *str, char const file[static 1],
                             int line) __attribute__((nonnull(1)));

static inline void *__growmem(void *restrict ptr, size_t count, size_t size,
                              size_t *capacity, char const file[static 1],
                              int line) __attribute__((nonnull(1, 4)));

static inline void free_if(void const *ptr)
{
    if (ptr)
        free((void *)ptr);
}

static inline void *__malloc(size_t size, char const file[static 1], int line)
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
                             size_t count, char const file[static 1], int line)
{
    void *ptr = memcpy(dest, src, count);
    if (!ptr)
    {
        __imm_log(IMM_LOG_FATAL, file, line, "failed to memcpy");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

static inline void *__realloc(void *ptr, size_t new_size,
                              char const file[static 1], int line)
{
    void *new_ptr = realloc(ptr, new_size);
    if (!new_ptr)
    {
        __imm_log(IMM_LOG_FATAL, file, line, "failed to realloc");
        exit(EXIT_FAILURE);
    }
    return new_ptr;
}

static inline char *__strdup(char const *str, char const file[static 1],
                             int line)
{
    char *new = strdup(str);
    if (!new)
    {
        __imm_log(IMM_LOG_FATAL, file, line, "failed to strdup");
        exit(EXIT_FAILURE);
    }
    return new;
}

static inline void *__growmem(void *restrict ptr, size_t count, size_t size,
                              size_t *capacity, char const file[static 1],
                              int line)
{
    if (size * count > *capacity)
    {
        (*capacity) <<= 1;
        IMM_BUG(*capacity < size * count);
        ptr = __realloc(ptr, *capacity, file, line);
    }
    return ptr;
}

#endif
