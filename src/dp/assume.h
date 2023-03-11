#ifndef DP_ASSUME_H
#define DP_ASSUME_H

#include "dp/premise.h"
#include "dp/state_type.h"

#define ASSUME(x) __builtin_assume(x)

#define ASSUME_MINLEN(P, x)                                                    \
    do                                                                         \
    {                                                                          \
        ASSUME(UNKNOWN_STATE <= P.state && P.state <= FRAME_STATE);            \
        if (P.state == UNKNOWN_STATE)                                          \
            ASSUME(0 <= x);                                                    \
        else if (P.state == MUTE_STATE)                                        \
            ASSUME(0 == x);                                                    \
        else if (P.state == NONMUTE_STATE)                                     \
            ASSUME(0 < x);                                                     \
        else if (P.state == NORMAL_STATE)                                      \
            ASSUME(1 == x);                                                    \
        else if (P.state == FRAME_STATE)                                       \
            ASSUME(P.min_len <= x);                                            \
        else                                                                   \
            __builtin_unreachable();                                           \
    } while (0);

#define ASSUME_MIXLEN(P, x, y)                                                 \
    do                                                                         \
    {                                                                          \
        ASSUME(UNKNOWN_STATE <= P.state && P.state <= FRAME_STATE);            \
        if (P.state == UNKNOWN_STATE)                                          \
            ASSUME(0 <= x && x <= y);                                          \
        else if (P.state == MUTE_STATE)                                        \
            ASSUME(0 == x && x == y);                                          \
        else if (P.state == NONMUTE_STATE)                                     \
            ASSUME(0 < x && x <= y);                                           \
        else if (P.state == NORMAL_STATE)                                      \
            ASSUME(1 == x && x == y);                                          \
        else if (P.state == FRAME_STATE)                                       \
            ASSUME(P.min_len <= x && x <= y && y <= P.max_len);                \
        else                                                                   \
            __builtin_unreachable();                                           \
    } while (0);

#endif
