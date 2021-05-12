#ifndef SUPPORT_H
#define SUPPORT_H

#include "imm/compiler.h"
#include "imm/log.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define BITS_PER_BYTE 8
#define BITS_TO_LONGS(nr) DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))

#define DIV_ROUND_UP(n, d) (((n) + (d)-1) / (d))

#define warn(code, ...) __imm_log(IMM_WARN, code, __VA_ARGS__)
#define error(code, ...) __imm_log(IMM_ERROR, code, __VA_ARGS__)
#define fatal(code, ...) __imm_log(IMM_FATAL, code, __VA_ARGS__)

#define xfwrite(buffer, size, count, stream)                                   \
    __fwrite((buffer), (size), (count), (stream), __FILE__, __LINE__, #buffer)

static inline void __fwrite(const void *restrict buffer, size_t size,
                            size_t count, FILE *restrict stream,
                            char const *file, int line, char const *buffstr)
{
    if (imm_unlikely(fwrite(buffer, size, count, stream) < count))
        error(IMM_IOERROR, "failed to write %s", buffstr);
}

static inline void bits_clr(unsigned long *x, unsigned bit)
{
    *x &= ~(1UL << bit);
}
static inline bool bits_get(unsigned long *x, unsigned bit)
{
    return !!((*x >> bit) & 1UL);
}
static inline void bits_set(unsigned long *x, unsigned bit)
{
    *x |= 1UL << bit;
}
static inline unsigned bits_width(uint32_t v)
{
    return v ? ((unsigned)__builtin_clz(v) ^ 31) + 1 : 0;
}

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
                              char const file[static 1], int line);

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
        fatal(IMM_IOERROR, "failed to malloc");
    return ptr;
}

static inline void *__memcpy(void *restrict dest, const void *restrict src,
                             size_t count, char const file[static 1], int line)
{
    void *ptr = memcpy(dest, src, count);
    if (!ptr)
        fatal(IMM_IOERROR, "failed to memcpy");
    return ptr;
}

static inline void *__realloc(void *ptr, size_t new_size,
                              char const file[static 1], int line)
{
    void *new_ptr = realloc(ptr, new_size);
    if (!new_ptr)
        fatal(IMM_IOERROR, "failed to realloc");
    return new_ptr;
}

static inline char *__strdup(char const *str, char const file[static 1],
                             int line)
{
    char *new = strdup(str);
    if (!new)
        fatal(IMM_IOERROR, "failed to strdup");
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

static inline unsigned long ipow(unsigned long base, unsigned exp)
{
    unsigned long result = 1;

    while (exp)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        base *= base;
    }

    return result;
}

#endif
