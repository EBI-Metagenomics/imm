#ifndef STD_COMPILER_H
#define STD_COMPILER_H

#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

#endif
