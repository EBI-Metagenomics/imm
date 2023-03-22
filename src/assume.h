#ifndef ASSUME_H
#define ASSUME_H

#include "compiler.h"

#if __has_builtin(__builtin_assume)
#define assume(x) __builtin_assume(x)
#else
#define assume(x)                                                              \
    do                                                                         \
    {                                                                          \
        if (!(x)) __builtin_unreachable();                                     \
    } while (0);
#endif

#endif
