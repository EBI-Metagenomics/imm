#ifndef COMMON_BUG_H
#define COMMON_BUG_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef NDEBUG
#define BUG(cond)
#else
#define BUG(cond)                                                              \
    do                                                                         \
    {                                                                          \
        if (!(bool)(cond))                                                     \
            break;                                                             \
        __bug_pexit(__FILE__, __func__, __LINE__, #cond);                      \
    } while (0)
#endif

static void __bug_pexit(char const *file, char const *func, int line,
                        char const *cond) __attribute__((noreturn));

static void __bug_pexit(char const *file, char const *func, int line,
                        char const *cond)
{
    fprintf(stderr, "BUG: %s: %s: %d: %s\n", file, func, line, cond);
    fflush(stderr);
    exit(EXIT_FAILURE);
}

#endif
