#ifndef DP_ASSUME_H
#define DP_ASSUME_H

#include "dp/state_type.h"

#define ASSUME_MINLEN(T, MINLEN, x)                                            \
    do                                                                         \
    {                                                                          \
        __builtin_assume(UNKNOWN_STATE_ID <= T && T <= FRAME_STATE_ID);        \
        if (T == UNKNOWN_STATE_ID)                                             \
            __builtin_assume(0 <= x);                                          \
        else if (T == MUTE_STATE_ID)                                           \
            __builtin_assume(0 == x);                                          \
        else if (T == NONMUTE_STATE_ID)                                        \
            __builtin_assume(0 < x);                                           \
        else if (T == NORMAL_STATE_ID)                                         \
            __builtin_assume(1 == x);                                          \
        else if (T == FRAME_STATE_ID)                                          \
            __builtin_assume(MINLEN <= x);                                     \
        else                                                                   \
            __builtin_unreachable();                                           \
    } while (0);

#define ASSUME_MIXLEN(T, MINLEN, MAXLEN, x, y)                                 \
    do                                                                         \
    {                                                                          \
        __builtin_assume(UNKNOWN_STATE_ID <= T && T <= FRAME_STATE_ID);        \
        if (T == UNKNOWN_STATE_ID)                                             \
            __builtin_assume(0 <= x && x <= y);                                \
        else if (T == MUTE_STATE_ID)                                           \
            __builtin_assume(0 == x && x == y);                                \
        else if (T == NONMUTE_STATE_ID)                                        \
            __builtin_assume(0 < x && x <= y);                                 \
        else if (T == NORMAL_STATE_ID)                                         \
            __builtin_assume(1 == x && x == y);                                \
        else if (T == FRAME_STATE_ID)                                          \
            __builtin_assume(MINLEN <= x && x <= y && y <= MAXLEN);            \
        else                                                                   \
            __builtin_unreachable();                                           \
    } while (0);

#define ASSUME_REAL_PAST(T, src, dst)                                          \
    do                                                                         \
    {                                                                          \
        if (T)                                                                 \
            __builtin_assume(src <= dst);                                      \
        else                                                                   \
            __builtin_assume(0 == 0);                                          \
    } while (0);

#define ASSUME_FIRST_ROW(T, row)                                               \
    do                                                                         \
    {                                                                          \
        if (T)                                                                 \
            __builtin_assume(row == 0);                                        \
        else                                                                   \
            __builtin_assume(0 == 0);                                          \
    } while (0);

#define ASSUME_SAFE_FLOOR(T, len, row)                                         \
    do                                                                         \
    {                                                                          \
        if (T)                                                                 \
            __builtin_assume(len <= row);                                      \
        else                                                                   \
            __builtin_assume(0 == 0);                                          \
    } while (0);

#define ASSUME_SAFE_CEIL(T, last_row, stop_row)                                \
    do                                                                         \
    {                                                                          \
        if (T)                                                                 \
            __builtin_assume(last_row <= stop_row);                            \
        else                                                                   \
            __builtin_assume(0 == 0);                                          \
    } while (0);

#endif
