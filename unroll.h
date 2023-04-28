#ifndef IMM_UNROLL_H
#define IMM_UNROLL_H

#define UNROLL_TO_STRING_HELPER(X) #X
#define UNROLL_TO_STRING(X) UNROLL_TO_STRING_HELPER(X)

// #if defined(__clang__)
// #define UNROLL _Pragma(UNROLL_TO_STRING(clang unroll(enable)))
// #elif defined(__GNUC__)
// #define UNROLL _Pragma(UNROLL_TO_STRING(GCC unroll 1))
// #endif

#define UNROLL(n) _Pragma(UNROLL_TO_STRING(GCC unroll n))

#endif
