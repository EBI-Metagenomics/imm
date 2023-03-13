#ifndef DP_ASSUME_H
#define DP_ASSUME_H

#include "dp/premise.h"
#include "dp/state_type.h"

#if __has_builtin(__builtin_assume)
#define ASSUME(x) __builtin_assume(x)
#else
#define ASSUME(x)                                                              \
    do                                                                         \
    {                                                                          \
        if (!(x)) __builtin_unreachable();                                     \
    } while (0);
#endif

#define ASSUME_MINLEN(state, x)                                                \
    do                                                                         \
    {                                                                          \
        ASSUME(UNKNOWN_STATE <= state && state <= FRAME15_STATE);              \
        if (state == UNKNOWN_STATE)                                            \
        {                                                                      \
            ASSUME(0 == 0);                                                    \
        }                                                                      \
        else if (state == MUTE_STATE)                                          \
        {                                                                      \
            ASSUME(0 == x);                                                    \
        }                                                                      \
        else if (state == NONMUTE_STATE)                                       \
        {                                                                      \
            ASSUME(0 < x);                                                     \
        }                                                                      \
        else if (state == NORMAL_STATE)                                        \
        {                                                                      \
            ASSUME(1 == x);                                                    \
        }                                                                      \
        else if (state == FRAME15_STATE)                                       \
        {                                                                      \
            ASSUME(1 <= x);                                                    \
        }                                                                      \
        else                                                                   \
            __builtin_unreachable();                                           \
    } while (0);

#define ASSUME_MIXLEN(state, x, y)                                             \
    do                                                                         \
    {                                                                          \
        ASSUME(UNKNOWN_STATE <= state && state <= FRAME15_STATE);              \
        if (state == UNKNOWN_STATE)                                            \
        {                                                                      \
            ASSUME(x <= y);                                                    \
        }                                                                      \
        else if (state == MUTE_STATE)                                          \
        {                                                                      \
            ASSUME(0 == x && x == y);                                          \
        }                                                                      \
        else if (state == NONMUTE_STATE)                                       \
        {                                                                      \
            ASSUME(0 < x && x <= y);                                           \
        }                                                                      \
        else if (state == NORMAL_STATE)                                        \
        {                                                                      \
            ASSUME(1 == x && x == y);                                          \
        }                                                                      \
        else if (state == FRAME15_STATE)                                       \
        {                                                                      \
            ASSUME(1 <= x && x <= y && y <= 5);                                \
        }                                                                      \
        else                                                                   \
            __builtin_unreachable();                                           \
    } while (0);

#endif
