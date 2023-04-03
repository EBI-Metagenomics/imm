#ifndef IMM_ASSUME_H
#define IMM_ASSUME_H

#include "compiler.h"
#include <assert.h>

#ifdef NDEBUG

#if __has_builtin(__builtin_assume)
#define imm_assume(x) __builtin_assume(x)
#else
#define imm_assume(x)                                                          \
  do                                                                           \
  {                                                                            \
    if (!(x)) __builtin_unreachable();                                         \
  } while (0);
#endif
#else
#define imm_assume(x) assert(x)
#endif

#endif
