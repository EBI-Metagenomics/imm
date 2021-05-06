#ifndef IMM_BUG_H
#define IMM_BUG_H

#include "imm/export.h"
#include <stdbool.h>

#ifdef NDEBUG
#define IMM_BUG(cond)
#else
#define IMM_BUG(cond)                                                          \
    do                                                                         \
    {                                                                          \
        if (!(bool)(cond))                                                     \
            break;                                                             \
        imm_bug(__FILE__, __func__, __LINE__, #cond);                          \
    } while (0)
#endif

IMM_API void imm_bug(char const *file, char const *func, int line,
                     char const *cond) __attribute__((noreturn));

#endif
