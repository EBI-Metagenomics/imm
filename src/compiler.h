#ifndef COMPILER_H
#define COMPILER_H

#define LIKELY(x)	__builtin_expect(!!(x), 1)
#define UNLIKELY(x)	__builtin_expect(!!(x), 0)

#endif
